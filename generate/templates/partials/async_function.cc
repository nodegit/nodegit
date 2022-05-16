
{%partial doc .%}
NAN_METHOD({{ cppClassName }}::{{ cppFunctionName }}) {
  {%partial guardArguments .%}
  if (!info[info.Length() - 1]->IsFunction()) {
    return Nan::ThrowError("Callback is required and must be a Function.");
  }

  {{ cppFunctionName }}Baton* baton = new {{ cppFunctionName }}Baton();

  baton->error_code = GIT_OK;
  baton->error = NULL;

  {%each args|argsInfo as arg %}
    {%if arg.globalPayload %}
      {{ cppFunctionName }}_globalPayload* globalPayload = new {{ cppFunctionName }}_globalPayload;
    {%endif%}
  {%endeach%}

  nodegit::Context *nodegitContext = reinterpret_cast<nodegit::Context *>(info.Data().As<External>()->Value());
  std::map<std::string, std::shared_ptr<nodegit::CleanupHandle>> cleanupHandles;

  {%each args|argsInfo as arg %}
    {%if not arg.isReturn %}
      {%if arg.isSelf %}
        baton->{{ arg.name }} = Nan::ObjectWrap::Unwrap<{{ arg.cppClassName }}>(info.This())->GetValue();
      {%elsif arg.isCallbackFunction %}
        if (!info[{{ arg.jsArg }}]->IsFunction()) {
          baton->{{ arg.name }} = NULL;
        {%if arg.payload.globalPayload %}
          globalPayload->{{ arg.name }} = NULL;
        {%else%}
          // NOTE this is a dead path
          baton->{{ arg.payload.name }} = NULL;
        {%endif%}
        }
        else {
          baton->{{ arg.name}} = {{ cppFunctionName }}_{{ arg.name }}_cppCallback;
          {%if arg.payload.globalPayload %}
            globalPayload->{{ arg.name }} = new Nan::Callback(info[{{ arg.jsArg }}].As<Function>());
          {%else%}
            // NOTE this is a dead path
            baton->{{ arg.payload.name }} = new Nan::Callback(info[{{ arg.jsArg }}].As<Function>());
          {%endif%}
        }
      {%elsif arg.payloadFor %}
        {%if arg.globalPayload %}
          baton->{{ arg.name }} = globalPayload;
        {%endif%}
      {% elsif arg.isStructType %}
        {% if arg.isOptional %}
          if (info[{{ arg.jsArg }}]->IsNull() || info[{{ arg.jsArg }}]->IsUndefined()) {
            baton->{{ arg.name }} = nullptr;
          } else
        {% endif %}
        {% if arg.cppClassName == 'Array' %}
          {
            v8::Local<v8::Array> tempArray = v8::Local<v8::Array>::Cast(info[{{ arg.jsArg }}]);
            baton->{{ arg.name }} = new {{ arg.cType|unPointer }}[tempArray->Length()];
            for (uint32_t i = 0; i < tempArray->Length(); ++i) {
              auto conversionResult = Configurable{{ arg.arrayElementCppClassName }}::fromJavascript(
                nodegitContext,
                Nan::Get(tempArray, i).ToLocalChecked()
              );

              if (!conversionResult.result) {
                delete[] baton->{{ arg.name }};
                return Nan::ThrowError(Nan::New(conversionResult.error).ToLocalChecked());
              }

              auto convertedObject = conversionResult.result;
              cleanupHandles["{{ arg.name }}"] = convertedObject;
              baton->{{ arg.name }}[i] = *convertedObject->GetValue();
            }
          }
        {% else %}
          {
            auto conversionResult = Configurable{{ arg.cppClassName }}::fromJavascript(nodegitContext, info[{{ arg.jsArg }}]);
            if (!conversionResult.result) {
              return Nan::ThrowError(Nan::New(conversionResult.error).ToLocalChecked());
            }

            auto convertedObject = conversionResult.result;
            cleanupHandles["{{ arg.name }}"] = convertedObject;
            baton->{{ arg.name }} = convertedObject->GetValue();
          }
        {% endif %}
      {%elsif arg.name %}
        {%partial convertFromV8 arg%}
        {%if not arg.payloadFor %}
          baton->{{ arg.name }} = from_{{ arg.name }};
          {%if arg | isOid %}
            baton->{{ arg.name }}NeedsFree = info[{{ arg.jsArg }}]->IsString();
          {%endif%}
        {%endif%}
      {%endif%}
    {%elsif arg.shouldAlloc %}
      baton->{{arg.name}} = ({{ arg.cType }})malloc(sizeof({{ arg.cType|replace '*' '' }}));
      {%if arg.cppClassName == "GitBuf" %}
        baton->{{arg.name}}->ptr = NULL;
        baton->{{arg.name}}->size = baton->{{arg.name}}->reserved = 0;
      {%endif%}
    {%endif%}
  {%endeach%}

  Nan::Callback *callback = new Nan::Callback(v8::Local<Function>::Cast(info[info.Length() - 1]));
  {{ cppFunctionName }}Worker *worker = new {{ cppFunctionName }}Worker(baton, callback, cleanupHandles);

  {%each args|argsInfo as arg %}
    {%if not arg.isReturn %}
      {%if arg.isSelf %}
        worker->Reference<{{ arg.cppClassName }}>("{{ arg.name }}", info.This());
      {%elsif not arg.isCallbackFunction %}
        {%if  arg.isUnwrappable %}
          {% if arg.cppClassName == "Array" %}
            if (info[{{ arg.jsArg }}]->IsArray()) {
              worker->Reference<{{ arg.arrayElementCppClassName }}>("{{ arg.name }}", info[{{ arg.jsArg }}].As<v8::Array>());
            }
          {% else %}
            worker->Reference<{{ arg.cppClassName }}>("{{ arg.name }}", info[{{ arg.jsArg }}]);
          {% endif %}
        {% else %}
          worker->Reference("{{ arg.name }}", info[{{ arg.jsArg }}]);
        {% endif %}
      {%endif%}
    {%endif%}
  {%endeach%}

  nodegitContext->QueueWorker(worker);
  return;
}

nodegit::LockMaster {{ cppClassName }}::{{ cppFunctionName }}Worker::AcquireLocks() {
  nodegit::LockMaster lockMaster(
    /*asyncAction: */true
    {%each args|argsInfo as arg %}
      {%if arg.cType|isPointer%}
        {%if not arg.cType|isDoublePointer%}
          ,baton->{{ arg.name }}
        {%endif%}
      {%endif%}
    {%endeach%}
  );

  return lockMaster;
}

void {{ cppClassName }}::{{ cppFunctionName }}Worker::Execute() {
  git_error_clear();

  {%if .|hasReturnType %}
    {{ return.cType }} result = {{ cFunctionName }}(
  {%else%}
    {{ cFunctionName }}(
  {%endif%}
    {%-- Insert Function Arguments --%}
    {%each args|argsInfo as arg %}
      {%-- turn the pointer into a ref --%}
      {%if arg.isReturn|and arg.cType|isDoublePointer %}&{%endif%}baton->{{ arg.name }}{%if not arg.lastArg %},{%endif%}
    {%endeach%}
  );

    {% if return.isResultOrError %}
      baton->error_code = result;
      if (result < GIT_OK && git_error_last() != NULL) {
        baton->error = git_error_dup(git_error_last());
      }

    {% elsif return.isErrorCode %}
      baton->error_code = result;

      if (result != GIT_OK && git_error_last() != NULL) {
        baton->error = git_error_dup(git_error_last());
      }

    {%elsif return.cType != 'void' %}

      baton->result = result;

    {%endif%}
}

void {{ cppClassName }}::{{ cppFunctionName }}Worker::HandleErrorCallback() {
  if (!GetIsCancelled()) {
    v8::Local<v8::Object> err = Nan::To<v8::Object>(Nan::Error(ErrorMessage())).ToLocalChecked();
    Nan::Set(err, Nan::New("errorFunction").ToLocalChecked(), Nan::New("{{ jsClassName }}.{{ jsFunctionName }}").ToLocalChecked());
    v8::Local<v8::Value> argv[1] = {
      err
    };
    callback->Call(1, argv, async_resource);
  }

  if (baton->error) {
    if (baton->error->message) {
      free((void *)baton->error->message);
    }

    free((void *)baton->error);
  }

  {%each args|argsInfo as arg %}
    {%if arg.shouldAlloc %}
      {%if not arg.isCppClassStringOrArray %}
      {%elsif arg | isOid %}
        if (baton->{{ arg.name}}NeedsFree) {
          baton->{{ arg.name}}NeedsFree = false;
          free((void*)baton->{{ arg.name }});
        }
      {%elsif arg.isCallbackFunction %}
        {%if not arg.payload.globalPayload %}
          delete baton->{{ arg.payload.name }};
        {%endif%}
      {%elsif arg.globalPayload %}
        delete ({{ cppFunctionName}}_globalPayload*)baton->{{ arg.name }};
      {%else%}
        free((void*)baton->{{ arg.name }});
      {%endif%}
    {%elsif arg.freeFunctionName|and arg.isReturn|and arg.selfFreeing %}
      {{ arg.freeFunctionName }}(baton->{{ arg.name }});
    {%endif%}
  {%endeach%}

  {%each args|argsInfo as arg %}
    {%if arg.isCppClassStringOrArray %}
      {%if arg.freeFunctionName %}
      {%elsif not arg.isConst%}
        free((void *)baton->{{ arg.name }});
      {%endif%}
    {%elsif arg | isOid %}
      if (baton->{{ arg.name}}NeedsFree) {
        baton->{{ arg.name}}NeedsFree = false;
        free((void *)baton->{{ arg.name }});
      }
    {%elsif arg.isCallbackFunction %}
      {%if not arg.payload.globalPayload %}
        delete baton->{{ arg.payload.name }};
      {%endif%}
    {%elsif arg.globalPayload %}
      delete ({{ cppFunctionName}}_globalPayload*)baton->{{ arg.name }};
    {%endif%}
    {%if arg.cppClassName == "GitBuf" %}
      {%if cppFunctionName == "Set" %}
      {%else%}
        git_buf_dispose(baton->{{ arg.name }});
        free((void *)baton->{{ arg.name }});
      {%endif%}
    {%endif%}
  {%endeach%}

  delete baton;
}

void {{ cppClassName }}::{{ cppFunctionName }}Worker::HandleOKCallback() {
  {%if return.isResultOrError %}
    if (baton->error_code >= GIT_OK) {
  {%else%}
    if (baton->error_code == GIT_OK) {
  {%endif%}

  {%if return.isResultOrError %}
    v8::Local<v8::Value> result = Nan::New<v8::Number>(baton->error_code);

  {%elsif not .|returnsCount %}
    v8::Local<v8::Value> result = Nan::Undefined();
  {%else%}
    v8::Local<v8::Value> v8ConversionSlot;
    {%if .|returnsCount > 1 %}
      v8::Local<Object> result = Nan::New<Object>();
    {%endif%}
    {%each .|returnsInfo 0 1 as _return %}
      {%partial convertToV8 _return %}
      {%if .|returnsCount > 1 %}
        Nan::Set(result, Nan::New("{{ _return.returnNameOrName }}").ToLocalChecked(), v8ConversionSlot);
      {%endif%}
    {%endeach%}
    {%if .|returnsCount == 1 %}
      v8::Local<v8::Value> result = v8ConversionSlot;
    {%endif%}
  {%endif%}

  {% each args|argsInfo as arg %}
    {% if not arg.ignore %}
      {% if arg.isStructType %}
        {% if arg.preserveOnThis %}
          {
            {% if args|thisInfo 'isReturn' %}
              auto objWrap = Nan::ObjectWrap::Unwrap<{{ args|thisInfo 'cppClassName' }}>(result.As<v8::Object>());
            {% else %}
              auto objWrap = Nan::ObjectWrap::Unwrap<{{ args|thisInfo 'cppClassName' }}>(GetFromPersistent("{{ args|thisInfo 'name' }}").As<v8::Object>());
            {% endif %}
            objWrap->SaveCleanupHandle(cleanupHandles["{{ arg.name }}"]);
          }
        {% endif %}
      {% endif %}
    {% endif %}
  {% endeach %}

    v8::Local<v8::Value> argv[2] = {
      Nan::Null(),
      result
    };
    callback->Call(2, argv, async_resource);
  } else {
    if (baton->error) {
      v8::Local<v8::Object> err;
      if (baton->error->message) {
        err = Nan::To<v8::Object>(Nan::Error(baton->error->message)).ToLocalChecked();
      } else {
        err = Nan::To<v8::Object>(Nan::Error("Method {{ jsFunctionName }} has thrown an error.")).ToLocalChecked();
      }
      Nan::Set(err, Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
      Nan::Set(err, Nan::New("errorFunction").ToLocalChecked(), Nan::New("{{ jsClassName }}.{{ jsFunctionName }}").ToLocalChecked());
      v8::Local<v8::Value> argv[1] = {
        err
      };
      callback->Call(1, argv, async_resource);
      if (baton->error->message)
        free((void *)baton->error->message);
      free((void *)baton->error);
    } else if (baton->error_code < 0) {
      bool callbackFired = false;
      if (!callbackErrorHandle.IsEmpty()) {
        v8::Local<v8::Value> maybeError = Nan::New(callbackErrorHandle);
        if (!maybeError->IsNull() && !maybeError->IsUndefined()) {
          v8::Local<v8::Value> argv[1] = {
            maybeError
          };
          callback->Call(1, argv, async_resource);
          callbackFired = true;
        }
      }

      if (!callbackFired) {
        v8::Local<v8::Object> err = Nan::To<v8::Object>(Nan::Error("Method {{ jsFunctionName }} has thrown an error.")).ToLocalChecked();
        Nan::Set(err, Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
        Nan::Set(err, Nan::New("errorFunction").ToLocalChecked(), Nan::New("{{ jsClassName }}.{{ jsFunctionName }}").ToLocalChecked());
        v8::Local<v8::Value> argv[1] = {
          err
        };
        callback->Call(1, argv, async_resource);
      }
    } else {
      callback->Call(0, NULL, async_resource);
    }

    {%each args|argsInfo as arg %}
      {%if arg.shouldAlloc %}
        {%if not arg.isCppClassStringOrArray %}
        {%elsif arg | isOid %}
          if (baton->{{ arg.name}}NeedsFree) {
            baton->{{ arg.name}}NeedsFree = false;
            free((void*)baton->{{ arg.name }});
          }
        {%elsif arg.isCallbackFunction %}
          {%if not arg.payload.globalPayload %}
            delete baton->{{ arg.payload.name }};
          {%endif%}
        {%elsif arg.globalPayload %}
          delete ({{ cppFunctionName}}_globalPayload*)baton->{{ arg.name }};
        {%else%}
          free((void*)baton->{{ arg.name }});
        {%endif%}
      {%elsif arg.freeFunctionName|and arg.isReturn|and arg.selfFreeing %}
        {{ arg.freeFunctionName }}(baton->{{ arg.name }});
      {%endif%}
    {%endeach%}
  }

  {%each args|argsInfo as arg %}
    {%if arg.isCppClassStringOrArray %}
      {%if arg.freeFunctionName %}
      {%elsif not arg.isConst%}
        free((void *)baton->{{ arg.name }});
      {%endif%}
    {%elsif arg | isOid %}
      if (baton->{{ arg.name}}NeedsFree) {
        baton->{{ arg.name}}NeedsFree = false;
        free((void *)baton->{{ arg.name }});
      }
    {%elsif arg.isCallbackFunction %}
      {%if not arg.payload.globalPayload %}
        delete baton->{{ arg.payload.name }};
      {%endif%}
    {%elsif arg.globalPayload %}
      delete ({{ cppFunctionName}}_globalPayload*)baton->{{ arg.name }};
    {%endif%}
    {%if arg.cppClassName == "GitBuf" %}
      {%if cppFunctionName == "Set" %}
      {%else%}
        git_buf_dispose(baton->{{ arg.name }});
        free((void *)baton->{{ arg.name }});
      {%endif%}
    {%endif%}
  {%endeach%}

  delete baton;
}

{%partial callbackHelpers .%}

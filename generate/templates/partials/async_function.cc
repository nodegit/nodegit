{%partial doc .%}
Napi::Value {{ cppClassName }}::{{ cppFunctionName }}(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);
  {%partial guardArguments .%}
  if (!info[info.Length() - 1].IsFunction()) {
    Napi::Error::New(env, "Callback is required and must be a Function.").ThrowAsJavaScriptException();
    return env.Undefined();
  }

  {{ cppFunctionName }}Baton* baton = new {{ cppFunctionName }}Baton();

  baton->error_code = GIT_OK;
  baton->error = NULL;

  {%each args|argsInfo as arg %}
    {%if arg.isReturn %}
      baton->{{ arg.name }} = NULL;
    {%endif%}
  {%endeach%}

  {%each args|argsInfo as arg %}
    {%if arg.globalPayload %}
      {{ cppFunctionName }}_globalPayload* globalPayload = new {{ cppFunctionName }}_globalPayload;
    {%endif%}
  {%endeach%}

  nodegit::Context *nodegitContext = nodegit::Context::GetCurrentContext();
  std::map<std::string, std::shared_ptr<nodegit::CleanupHandle>> cleanupHandles;

  {%each args|argsInfo as arg %}
    {%if not arg.isReturn %}
      {%if arg.isSelf %}
        baton->{{ arg.name }} = NodeGitWrapper<{{ arg.cppClassName }}Traits>::Unwrap<{{ arg.cppClassName }}>(info.This().As<Napi::Object>())->GetValue();
      {%elsif arg.isCallbackFunction %}
        if (!info[{{ arg.jsArg }}].IsFunction()) {
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
            globalPayload->{{ arg.name }} = new Napi::FunctionReference(Napi::Persistent(info[{{ arg.jsArg }}].As<Napi::Function>()));
          {%else%}
            // NOTE this is a dead path
            baton->{{ arg.payload.name }} = new Napi::FunctionReference(Napi::Persistent(info[{{ arg.jsArg }}].As<Napi::Function>()));
          {%endif%}
        }
      {%elsif arg.payloadFor %}
        {%if arg.globalPayload %}
          baton->{{ arg.name }} = globalPayload;
        {%endif%}
      {% elsif arg.isStructType %}
        {% if arg.isOptional %}
          if (info[{{ arg.jsArg }}].IsNull() || info[{{ arg.jsArg }}].IsUndefined()) {
            baton->{{ arg.name }} = nullptr;
          } else
        {% endif %}
        {% if arg.cppClassName == 'Array' %}
          {
            Napi::Array tempArray = info[{{ arg.jsArg }}].As<Napi::Array>();
            baton->{{ arg.name }} = ({{ arg.cType|unPointer }}*)malloc(sizeof({{ arg.cType|unPointer }}) * tempArray.Length());
            for (uint32_t i = 0; i < tempArray.Length(); ++i) {
              auto conversionResult = Configurable{{ arg.arrayElementCppClassName }}::fromJavascript(
                nodegitContext,
                tempArray.Get(i)
              );

              if (!conversionResult.result) {
                // TODO free previously allocated memory
                free(baton->{{ arg.name }});
                Napi::Error::New(env, conversionResult.error).ThrowAsJavaScriptException();
                return env.Undefined();
              }

              auto convertedObject = conversionResult.result;
              cleanupHandles[std::string("{{ arg.name }}") + std::to_string(i)] = convertedObject;
              baton->{{ arg.name }}[i] = *convertedObject->GetValue();
            }
          }
        {% else %}
          {
            auto conversionResult = Configurable{{ arg.cppClassName }}::fromJavascript(nodegitContext, info[{{ arg.jsArg }}]);
            if (!conversionResult.result) {
              Napi::Error::New(env, conversionResult.error).ThrowAsJavaScriptException();
              return env.Undefined();
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
            baton->{{ arg.name }}NeedsFree = info[{{ arg.jsArg }}].IsString();
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

  Napi::FunctionReference *callback = new Napi::FunctionReference(Napi::Persistent(info[info.Length() - 1].As<Napi::Function>()));
  {{ cppFunctionName }}Worker *worker = new {{ cppFunctionName }}Worker(baton, callback, cleanupHandles);

  {%each args|argsInfo as arg %}
    {%if not arg.isReturn %}
      {%if arg.isSelf %}
        worker->Reference<{{ arg.cppClassName }}>("{{ arg.name }}", info.This().As<Napi::Object>());
      {%elsif not arg.isCallbackFunction %}
        {%if  arg.isUnwrappable %}
          {% if arg.cppClassName == "Array" %}
            if (info[{{ arg.jsArg }}].IsArray()) {
              worker->Reference<{{ arg.arrayElementCppClassName }}>("{{ arg.name }}", info[{{ arg.jsArg }}].As<Napi::Array>());
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
  return env.Undefined();
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
      if (result < GIT_OK && git_error_last()->klass != GIT_ERROR_NONE) {
        baton->error = git_error_dup(git_error_last());
      }

    {% elsif return.isErrorCode %}
      baton->error_code = result;

      if (result != GIT_OK && git_error_last()->klass != GIT_ERROR_NONE) {
        baton->error = git_error_dup(git_error_last());
      }

    {%elsif return.cType != 'void' %}

      baton->result = result;

    {%endif%}
}

void {{ cppClassName }}::{{ cppFunctionName }}Worker::HandleErrorCallback() {
  Napi::Env env = GetAsyncResource()->Env();
  Napi::HandleScope scope(env);
  if (!GetIsCancelled()) {
    Napi::Object err = Napi::Error::New(env, ErrorMessage()).Value();
    err.Set("errorFunction", Napi::String::New(env, "{{ jsClassName }}.{{ jsFunctionName }}"));
    Napi::Value argv[1] = {
      err
    };
    CallCallback(argv, 1);
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
  Napi::Env env = GetAsyncResource()->Env();
  Napi::HandleScope scope(env);
  {%if return.isResultOrError %}
    if (baton->error_code >= GIT_OK) {
  {%else%}
    if (baton->error_code == GIT_OK) {
  {%endif%}

  {%if return.isResultOrError %}
    Napi::Value result = Napi::Number::New(env, baton->error_code);

  {%elsif not .|returnsCount %}
    Napi::Value result = env.Undefined();
  {%else%}
    Napi::Value v8ConversionSlot;
    {%if .|returnsCount > 1 %}
      Napi::Object result = Napi::Object::New(env);
    {%endif%}
    {%each .|returnsInfo 0 1 as _return %}
      {%partial convertToV8 _return %}
      {%if .|returnsCount > 1 %}
        result.Set("{{ _return.returnNameOrName }}", v8ConversionSlot);
      {%endif%}
    {%endeach%}
    {%if .|returnsCount == 1 %}
      Napi::Value result = v8ConversionSlot;
    {%endif%}
  {%endif%}

  {% each args|argsInfo as arg %}
    {% if not arg.ignore %}
      {% if arg.isStructType %}
        {% if arg.preserveOnThis %}
          {
            {% if args|thisInfo 'isReturn' %}
              auto objWrap = NodeGitWrapper<{{ args|thisInfo 'cppClassName' }}Traits>::Unwrap<{{ args|thisInfo 'cppClassName' }}>(result.As<Napi::Object>());
            {% else %}
              auto objWrap = NodeGitWrapper<{{ args|thisInfo 'cppClassName' }}Traits>::Unwrap<{{ args|thisInfo 'cppClassName' }}>(GetFromPersistent("{{ args|thisInfo 'name' }}").As<Napi::Object>());
            {% endif %}
            objWrap->SaveCleanupHandle(cleanupHandles["{{ arg.name }}"]);
          }
        {% endif %}
      {% endif %}
    {% endif %}
  {% endeach %}

    Napi::Value argv[2] = {
      env.Null(),
      result
    };
    CallCallback(argv, 2);
  } else {
    if (baton->error) {
      Napi::Object err;
      if (baton->error->message) {
        err = Napi::Error::New(env, baton->error->message).Value();
      } else {
        err = Napi::Error::New(env, "Method {{ jsFunctionName }} has thrown an error.").Value();
      }
      err.Set("errno", Napi::Number::New(env, baton->error_code));
      err.Set("errorFunction", Napi::String::New(env, "{{ jsClassName }}.{{ jsFunctionName }}"));
      Napi::Value argv[1] = {
        err
      };
      CallCallback(argv, 1);
      if (baton->error->message)
        free((void *)baton->error->message);
      free((void *)baton->error);
    } else if (baton->error_code < 0) {
      bool callbackFired = false;
      if (!callbackErrorHandle.IsEmpty()) {
        Napi::Value maybeError = callbackErrorHandle.Value();
        if (!maybeError.IsNull() && !maybeError.IsUndefined()) {
          Napi::Value argv[1] = {
            maybeError
          };
          CallCallback(argv, 1);
          callbackFired = true;
        }
      }

      if (!callbackFired) {
        Napi::Object err = Napi::Error::New(env, "Method {{ jsFunctionName }} has thrown an error.").Value();
        err.Set("errno", Napi::Number::New(env, baton->error_code));
        err.Set("errorFunction", Napi::String::New(env, "{{ jsClassName }}.{{ jsFunctionName }}"));
        Napi::Value argv[1] = {
          err
        };
        CallCallback(argv, 1);
      }
    } else {
      CallCallback(nullptr, 0);
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

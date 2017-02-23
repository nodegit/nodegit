
{%partial doc .%}
NAN_METHOD({{ cppClassName }}::{{ cppFunctionName }}) {
  {%partial guardArguments .%}
  if (info.Length() == {{args|jsArgsCount}} || !info[{{args|jsArgsCount}}]->IsFunction()) {
    return Nan::ThrowError("Callback is required and must be a Function.");
  }

  {{ cppFunctionName }}Baton* baton = new {{ cppFunctionName }}Baton;

  baton->error_code = GIT_OK;
  baton->error = NULL;

  {%each args|argsInfo as arg %}
    {%if arg.globalPayload %}
  {{ cppFunctionName }}_globalPayload* globalPayload = new {{ cppFunctionName }}_globalPayload;
    {%endif%}
    {%if arg.cppClassName == "GitBuf" %}
      baton->{{arg.name}} = ({{ arg.cType }})malloc(sizeof({{ arg.cType|replace '*' '' }}));;
      baton->{{arg.name}}->ptr = NULL;
      baton->{{arg.name}}->size = baton->{{arg.name}}->asize = 0;
    {%endif%}
  {%endeach%}

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
    baton->{{ arg.payload.name }} = NULL;
        {%endif%}
  }
  else {
    baton->{{ arg.name}} = {{ cppFunctionName }}_{{ arg.name }}_cppCallback;
        {%if arg.payload.globalPayload %}
    globalPayload->{{ arg.name }} = new Nan::Callback(info[{{ arg.jsArg }}].As<Function>());
        {%else%}
    baton->{{ arg.payload.name }} = new Nan::Callback(info[{{ arg.jsArg }}].As<Function>());
        {%endif%}
  }
      {%elsif arg.payloadFor %}
        {%if arg.globalPayload %}
  baton->{{ arg.name }} = globalPayload;
        {%endif%}
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
  baton->{{ arg.name }} = ({{ arg.cType }})malloc(sizeof({{ arg.cType|replace '*' '' }}));
    {%endif%}
  {%endeach%}

  Nan::Callback *callback = new Nan::Callback(v8::Local<Function>::Cast(info[{{args|jsArgsCount}}]));
  {{ cppFunctionName }}Worker *worker = new {{ cppFunctionName }}Worker(baton, callback);
  {%each args|argsInfo as arg %}
    {%if not arg.isReturn %}
      {%if arg.isSelf %}
  worker->SaveToPersistent("{{ arg.name }}", info.This());
      {%elsif not arg.isCallbackFunction %}
  if (!info[{{ arg.jsArg }}]->IsUndefined() && !info[{{ arg.jsArg }}]->IsNull())
    worker->SaveToPersistent("{{ arg.name }}", info[{{ arg.jsArg }}]->ToObject());
      {%endif%}
    {%endif%}
  {%endeach%}

  AsyncLibgit2QueueWorker(worker);
  return;
}

void {{ cppClassName }}::{{ cppFunctionName }}Worker::Execute() {
  giterr_clear();

  {
    LockMaster lockMaster(/*asyncAction: */true{%each args|argsInfo as arg %}
      {%if arg.cType|isPointer%}{%if not arg.cType|isDoublePointer%}
        ,baton->{{ arg.name }}
      {%endif%}{%endif%}
    {%endeach%});

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

    {%if return.isResultOrError %}
    baton->error_code = result;
    if (result < GIT_OK && giterr_last() != NULL) {
      baton->error = git_error_dup(giterr_last());
    }

    {%elsif return.isErrorCode %}
    baton->error_code = result;

    if (result != GIT_OK && giterr_last() != NULL) {
      baton->error = git_error_dup(giterr_last());
    }

    {%elsif not return.cType == 'void' %}

    baton->result = result;

    {%endif%}
  }
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
    v8::Local<v8::Value> to;
      {%if .|returnsCount > 1 %}
    v8::Local<Object> result = Nan::New<Object>();
      {%endif%}
      {%each .|returnsInfo 0 1 as _return %}
        {%partial convertToV8 _return %}
        {%if .|returnsCount > 1 %}
    Nan::Set(result, Nan::New("{{ _return.returnNameOrName }}").ToLocalChecked(), to);
        {%endif%}
      {%endeach%}
      {%if .|returnsCount == 1 %}
    v8::Local<v8::Value> result = to;
      {%endif%}
    {%endif%}
    v8::Local<v8::Value> argv[2] = {
      Nan::Null(),
      result
    };
    callback->Call(2, argv);
  } else {
    if (baton->error) {
      v8::Local<v8::Value> argv[1] = {
        Nan::Error(baton->error->message)
      };
      callback->Call(1, argv);
      if (baton->error->message)
        free((void *)baton->error->message);
      free((void *)baton->error);
    } else if (baton->error_code < 0) {
      std::queue< v8::Local<v8::Value> > workerArguments;
{%each args|argsInfo as arg %}
  {%if not arg.isReturn %}
    {%if not arg.isSelf %}
      {%if not arg.isCallbackFunction %}
      workerArguments.push(GetFromPersistent("{{ arg.name }}"));
      {%endif%}
    {%endif%}
  {%endif%}
{%endeach%}
      bool callbackFired = false;
      while(!workerArguments.empty()) {
        v8::Local<v8::Value> node = workerArguments.front();
        workerArguments.pop();

        if (
          !node->IsObject()
          || node->IsArray()
          || node->IsBooleanObject()
          || node->IsDate()
          || node->IsFunction()
          || node->IsNumberObject()
          || node->IsRegExp()
          || node->IsStringObject()
        ) {
          continue;
        }

        v8::Local<v8::Object> nodeObj = node->ToObject();
        v8::Local<v8::Value> checkValue = GetPrivate(nodeObj, Nan::New("NodeGitPromiseError").ToLocalChecked());

        if (!checkValue.IsEmpty() && !checkValue->IsNull() && !checkValue->IsUndefined()) {
          v8::Local<v8::Value> argv[1] = {
            checkValue->ToObject()
          };
          callback->Call(1, argv);
          callbackFired = true;
          break;
        }

        v8::Local<v8::Array> properties = nodeObj->GetPropertyNames();
        for (unsigned int propIndex = 0; propIndex < properties->Length(); ++propIndex) {
          v8::Local<v8::String> propName = properties->Get(propIndex)->ToString();
          v8::Local<v8::Value> nodeToQueue = nodeObj->Get(propName);
          if (!nodeToQueue->IsUndefined()) {
            workerArguments.push(nodeToQueue);
          }
        }
      }

      if (!callbackFired) {
        v8::Local<v8::Object> err = Nan::Error("Method {{ jsFunctionName }} has thrown an error.")->ToObject();
        err->Set(Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
        v8::Local<v8::Value> argv[1] = {
          err
        };
        callback->Call(1, argv);
      }
    } else {
      callback->Call(0, NULL);
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
      {%endif%}
    {%endeach%}
  }

  {%each args|argsInfo as arg %}
    {%if arg.isCppClassStringOrArray %}
      {%if arg.freeFunctionName %}
  {{ arg.freeFunctionName }}(baton->{{ arg.name }});
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
  git_buf_free(baton->{{ arg.name }});
  free((void *)baton->{{ arg.name }});
    {%endif%}
  {%endeach%}

  delete baton;
}

{%partial callbackHelpers .%}

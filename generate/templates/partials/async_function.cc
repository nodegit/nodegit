
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

  Nan::Callback *callback = new Nan::Callback(Local<Function>::Cast(info[{{args|jsArgsCount}}]));
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

  Nan::AsyncQueueWorker(worker);
  return;
}

void {{ cppClassName }}::{{ cppFunctionName }}Worker::Execute() {
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

  {%if return.isErrorCode %}
  baton->error_code = result;

  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }

  {%elsif not return.cType == 'void' %}

  baton->result = result;

  {%endif%}
}

void {{ cppClassName }}::{{ cppFunctionName }}Worker::HandleOKCallback() {
  if (baton->error_code == GIT_OK) {
    {%if not .|returnsCount %}
    Local<v8::Value> result = Nan::Undefined();
    {%else%}
    Local<v8::Value> to;
      {%if .|returnsCount > 1 %}
    Local<Object> result = Nan::New<Object>();
      {%endif%}
      {%each .|returnsInfo 0 1 as _return %}
        {%partial convertToV8 _return %}
        {%if .|returnsCount > 1 %}
    Nan::Set(result, Nan::New("{{ _return.returnNameOrName }}").ToLocalChecked(), to);
        {%endif%}
      {%endeach%}
      {%if .|returnsCount == 1 %}
    Local<v8::Value> result = to;
      {%endif%}
    {%endif%}
    Local<v8::Value> argv[2] = {
      Nan::Null(),
      result
    };
    callback->Call(2, argv);
  } else {
    if (baton->error) {
      Local<v8::Value> argv[1] = {
        Nan::Error(baton->error->message)
      };
      callback->Call(1, argv);
      if (baton->error->message)
        free((void *)baton->error->message);
      free((void *)baton->error);
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
      {%else%}
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

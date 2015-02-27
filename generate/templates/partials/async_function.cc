
{%partial doc .%}
NAN_METHOD({{ cppClassName }}::{{ cppFunctionName }}) {
  NanScope();
  {%partial guardArguments .%}
  if (args.Length() == {{args|jsArgsCount}} || !args[{{args|jsArgsCount}}]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  {{ cppFunctionName }}Baton* baton = new {{ cppFunctionName }}Baton;

  baton->error_code = GIT_OK;
  baton->error = NULL;

  {%each args|argsInfo as arg %}
    {%if arg.globalPayload %}
  {{ cppFunctionName }}_globalPayload* globalPayload = new {{ cppFunctionName }}_globalPayload;
    {%endif%}
  {%endeach%}

  {%each args|argsInfo as arg %}
    {%if not arg.isReturn %}
      {%if arg.isSelf %}
  baton->{{ arg.name }} = ObjectWrap::Unwrap<{{ arg.cppClassName }}>(args.This())->GetValue();
      {%elsif arg.isCallbackFunction %}
  if (!args[{{ arg.jsArg }}]->IsFunction()) {
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
    globalPayload->{{ arg.name }} = new PayloadWrapper();
    globalPayload->{{ arg.name }}->jsCallback = new NanCallback(args[{{ arg.jsArg }}].As<Function>());
    uv_async_init(uv_default_loop(), &globalPayload->{{ arg.name }}->handle, {{ cppFunctionName }}_{{ arg.name }}_async);
        {%else%}
    baton->{{ arg.payload.name }} = new PayloadWrapper();
    baton->{{ arg.payload.name }}->jsCallback = new NanCallback(args[{{ arg.jsArg }}].As<Function>());
    uv_async_init(uv_default_loop(), &baton->{{ arg.payload.name }}->handle, {{ cppFunctionName }}_{{ arg.name }}_async);
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
  baton->{{ arg.name }}NeedsFree = args[{{ arg.jsArg }}]->IsString();
          {%endif%}
        {%endif%}
      {%endif%}
    {%elsif arg.shouldAlloc %}
  baton->{{ arg.name }} = ({{ arg.cType }})malloc(sizeof({{ arg.cType|replace '*' '' }}));
    {%endif%}
  {%endeach%}

  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[{{args|jsArgsCount}}]));
  {{ cppFunctionName }}Worker *worker = new {{ cppFunctionName }}Worker(baton, callback);
  {%each args|argsInfo as arg %}
    {%if not arg.isReturn %}
      {%if arg.isSelf %}
  worker->SaveToPersistent("{{ arg.name }}", args.This());
      {%elsif not arg.isCallbackFunction %}
  if (!args[{{ arg.jsArg }}]->IsUndefined() && !args[{{ arg.jsArg }}]->IsNull())
    worker->SaveToPersistent("{{ arg.name }}", args[{{ arg.jsArg }}]->ToObject());
      {%endif%}
    {%endif%}
  {%endeach%}

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
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
  TryCatch try_catch;

  if (baton->error_code == GIT_OK) {
    {%if not .|returnsCount %}
    Handle<v8::Value> result = NanUndefined();
    {%else%}
    Handle<v8::Value> to;
      {%if .|returnsCount > 1 %}
    Handle<Object> result = NanNew<Object>();
      {%endif%}
      {%each .|returnsInfo 0 1 as _return %}
        {%partial convertToV8 _return %}
        {%if .|returnsCount > 1 %}
    result->Set(NanNew<String>("{{ _return.returnNameOrName }}"), to);
        {%endif%}
      {%endeach%}
      {%if .|returnsCount == 1 %}
    Handle<v8::Value> result = to;
      {%endif%}
    {%endif%}
    Handle<v8::Value> argv[2] = {
      NanNull(),
      result
    };
    callback->Call(2, argv);
  } else {
    if (baton->error) {
      Handle<v8::Value> argv[1] = {
        NanError(baton->error->message)
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

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
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
  {%endeach%}

  delete baton;
}

{%partial callbackHelpers .%}

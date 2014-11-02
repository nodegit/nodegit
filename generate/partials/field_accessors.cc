{%each fields|fieldsInfo as field %}
  {%if not field.ignore %}

NAN_GETTER({{ cppClassName }}::Get{{ field.cppFunctionName }}) {
  NanScope();

  {{ cppClassName }} *wrapper = ObjectWrap::Unwrap<{{ cppClassName }}>(args.This());

  {%if field.isEnum %}
  NanReturnValue(NanNew((int)wrapper->GetValue()->{{ field.name }}));
  {%elsif field.isLibgitType | or field.payloadFor %}
  NanReturnValue(wrapper->{{ field.name }});
  {%elsif field.isCallbackFunction %}
  NanReturnValue(wrapper->{{ field.name }}->GetFunction());
  {%elsif field.cppClassName == 'String' %}
  if (wrapper->GetValue()->{{ field.name }}) {
    NanReturnValue(NanNew<String>(wrapper->GetValue()->{{ field.name }}));
  }
  else {
    NanReturnUndefined();
  }
  {%elsif field.cppClassName|isV8Value %}
  NanReturnValue(NanNew<{{ field.cppClassName }}>(wrapper->GetValue()->{{ field.name }}));
  {%endif%}
}

NAN_SETTER({{ cppClassName }}::Set{{ field.cppFunctionName }}) {
  NanScope();

  {{ cppClassName }} *wrapper = ObjectWrap::Unwrap<{{ cppClassName }}>(args.This());

  {%if field.isEnum %}
  if (value->IsNumber()) {
    wrapper->GetValue()->{{ field.name }} = ({{ field.cType }}) value->Int32Value();
  }

  {%elsif field.isLibgitType %}
  NanDisposePersistent(wrapper->{{ field.name }});

  wrapper->raw->{{ field.name }} = {%if not field.cType | isPointer %}*{%endif%}ObjectWrap::Unwrap<{{ field.cppClassName }}>(value->ToObject())->GetValue();
  {%elsif field.isCallbackFunction %}
  if (value->IsFunction()) {
    wrapper->{{ field.name }} = new NanCallback(value.As<Function>());
  }
  {%elsif field.payloadFor %}
  NanAssignPersistent(wrapper->{{ field.name }}, value);
  {%elsif field.cppClassName == 'String' %}
  if (wrapper->GetValue()->{{ field.name }}) {
    //free(wrapper->{{ field.name }});
  }

  String::Utf8Value str(value);
  wrapper->GetValue()->{{ field.name }} = strdup(*str);
  {%elsif field.isCppClassIntType%}
  if (value->IsNumber()) {
    wrapper->GetValue()->{{ field.name }} = value->{{field.cppClassName}}Value();
  }
  {%else%}
  if (value->IsNumber()) {
    wrapper->GetValue()->{{ field.name }} = ({{ field.cType }}) value->Int32Value();
  }
  {%endif%}
}

    {%if field.isCallbackFunction %}
{{ field.returnType }} {{ cppClassName }}::{{ field.name }}_cppCallback (
  {%each field.args|argsInfo as arg%}
  {{ arg.cType }} {{ arg.name}}{%if not arg.lastArg %},{%endif%}

  {%endeach%}
  ) {
  {{ field.name|titleCase }}Baton* baton = new {{ field.name|titleCase }}Baton();

  {%each field.args|argsInfo as arg %}
  baton->{{ arg.name }} = {{ arg.name }};
  {%endeach%}
  baton->req.data = baton;
  baton->done = false;

  uv_queue_work(uv_default_loop(), &baton->req, {{ field.name }}_asyncWork, {{ field.name }}_asyncAfter);

  while(!baton->done) {
    this_thread::sleep_for(chrono::milliseconds(1));
  }

  {%each field|returnsInfo true false as _return %}
  *{{ _return.name }} = *baton->{{ _return.name }};
  {%endeach%}

  return baton->result;
}

void {{ cppClassName }}::{{ field.name }}_asyncWork(uv_work_t* req) {
  // We aren't doing any work on a seperate thread, just need to
  // access the main node thread in the async after method.
  // However, this worker method is still needed
}

void {{ cppClassName }}::{{ field.name }}_asyncAfter(uv_work_t* req, int status) {
  NanScope();

  {{ field.name|titleCase }}Baton* baton = static_cast<{{ field.name|titleCase }}Baton*>(req->data);
  {{ cppClassName }}* instance = static_cast<{{ cppClassName }}*>(baton->payload);

  if (instance->{{ field.name }}->IsEmpty()) {
    {%if field.returnType == "int" %}
    baton->result = {{ field.returnNoResults }}; // no results acquired
    {%endif%}
    baton->done = true;
    return;
  }

  Local<Value> argv[{{ field.args|jsArgsCount }}] = {
    {%each field.args|argsInfo as arg %}
      {%if arg.name == "payload" %}
      {%-- payload is always the last arg --%}
    NanNew(instance->{{ fields|payloadFor field.name }})
      {%elsif arg.isJsArg %}
        {%if arg.isEnum %}
    NanNew((int)baton->{{ arg.name }}),
        {%elsif arg.isLibgitType %}
    NanNew({{ arg.cppClassName }}::New(&baton->{{ arg.name }}, false)),
        {%elsif arg.cType == "size_t" %}
    // HACK: NAN should really have an overload for NanNew to support size_t
    NanNew((unsigned int)baton->{{ arg.name }}),
        {%else%}
    NanNew(baton->{{ arg.name }}),
        {%endif%}
      {%endif%}
    {%endeach%}
  };

  TryCatch tryCatch;
  Handle<Value> result = instance->{{ field.name }}->Call({{ field.args|jsArgsCount }}, argv);
  {{ field.returnType }} resultStatus;

  {%each field|returnsInfo true false as _return%}
  if (result.IsEmpty() || result->IsNativeError()) {
    baton->result = {{ field.returnError }};
  }
  else if (!result->IsNull() && !result->IsUndefined()) {
    {{ _return.cppClassName }}* wrapper = ObjectWrap::Unwrap<{{ _return.cppClassName }}>(result->ToObject());
    wrapper->selfFreeing = false;

    baton->{{ _return.name }} = wrapper->GetRefValue();
    baton->result = {{ field.returnSuccess }};
  }
  else {
    baton->result = {{ field.returnNoResults }};
  }
  {%endeach%}
  baton->done = true;
}
    {%endif%}
  {%endif%}
{%endeach%}

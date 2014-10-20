{%each fields|fieldsInfo as field %}
  {%if not field.ignore %}

NAN_GETTER({{ cppClassName }}::Get{{ field.cppFunctionName }}) {
  NanScope();

  {{ cppClassName }} *wrapper = ObjectWrap::Unwrap<{{ cppClassName }}>(args.This());

  {%if field.hasConstructor | or field.isFunction | or field.payloadFor %}
  NanReturnValue(wrapper->{{ field.name }});
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

  {%if field.hasConstructor %}
  wrapper->{{ field.name }}.Dispose();
  wrapper->{{ field.name }}.Clear();

  wrapper->{{ field.name }} = Persistent<Object>::New(value->ToObject());
  wrapper->raw->{{ field.name }} = *ObjectWrap::Unwrap<{{ field.cppClassName }}>(value->ToObject())->GetValue();
  {%elsif field.isFunction | or field.payloadFor %}
  if (value->IsFunction()) {
    wrapper->{{ field.name }}.Dispose();
    wrapper->{{ field.name }}.Clear();
    wrapper->{{ field.name }} = Persistent<Value>::New(value);
  }
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

    {%if field.isFunction %}
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
  {{ _return.name }} = baton->{{ _return.name }};
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

  if (!instance->{{ field.name }}->IsFunction()) {
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
    Local<Value>::New(instance->{{ fields|payloadFor field.name }})
      {%elsif arg.isJsArg %}
    Local<Value>::New(NanNew(baton->{{ arg.name }})),

      {%endif%}
    {%endeach%}
  };

  Local<Value> result = Local<Value>::New(Function::Cast(*instance->{{ field.name }})->Call(Context::GetCurrent()->Global(), {{ field.args|jsArgsCount }}, argv));
  {{ field.returnType }} resultStatus;

  {%each field|returnsInfo true false as _return%}
  if (!result->IsNull() && !result->IsUndefined()) {
    {{ _return.cppClassName }}* wrapper = ObjectWrap::Unwrap<{{ _return.cppClassName }}>(result->ToObject());
    wrapper->selfFreeing = false;

    baton->{{ _return.name }} = wrapper->GetRefValue();
    baton->result = {{ field.returnSuccess }};
  }
  else if (result->IsNativeError()) {
    baton->result = {{ field.returnError }};
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

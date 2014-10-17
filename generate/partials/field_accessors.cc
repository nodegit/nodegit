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

  {%if field.hasConstructor | or field.payloadFor %}
  wrapper->{{ field.name }}.Dispose();
  wrapper->{{ field.name }}.Clear();

  wrapper->{{ field.name }} = Persistent<Object>::New(value->ToObject());
    {%if field.hasConstructor %}
  wrapper->raw->{{ field.name }} = *ObjectWrap::Unwrap<{{ field.cppClassName }}>(value->ToObject())->GetValue();
    {%endif%}
  {%elsif field.isFunction %}
  if (value->IsFunction()) {
    wrapper->{{ field.name }}.Dispose();
    wrapper->{{ field.name }}.Clear();
    wrapper->{{ field.name }} = Persistent<Function>::Cast((Persistent<Value>)value);
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
  {{ cppClassName }} *instance = ({{ cppClassName }}*)payload;

  if (!instance->{{ field.name }}->IsFunction()) {
    {%if field.returnType == "int" %}
    return {{ field.returnNoResults }}; // no results acquired
    {%else%}
    return;
    {%endif%}
  }

  Local<Value> argv[{{ field.args|jsArgsCount }}] = {
    {%each field.args|argsInfo as arg %}
      {%if arg.name == "payload" %}
      {%-- payload is always the last arg --%}
    Local<Value>::New(NanNew(instance->{{ fields|payloadFor field.name }}))
      {%elsif arg.isJsArg %}
    Local<Value>::New(NanNew({{ arg.name }})),

      {%endif%}
    {%endeach%}
  };

  Persistent<Value> result = Persistent<Value>::New(instance->{{ field.name }}->Call(Context::GetCurrent()->Global(), {{ field.args|jsArgsCount }}, argv));
  {{ field.returnType }} resultStatus;

  {%each field|returnsInfo true false as _return%}
  if (result->IsObject()) {
    {{ _return.cppClassName }}* wrapper = ObjectWrap::Unwrap<{{ _return.cppClassName }}>(result->ToObject());
    wrapper->selfFreeing = false;

    {{ _return.name }} = wrapper->GetRefValue();
    resultStatus = {{ field.returnSuccess }};
  }
  else if (result->IsNativeError()) {
    resultStatus = {{ field.returnError }};
  }
  else {
    resultStatus = {{ field.returnNoResults }};
  }

  result.Dispose();
  result.Clear();

  return resultStatus;
  {%endeach%}
}
    {%endif%}
  {%endif%}
{%endeach%}

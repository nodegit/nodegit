{%each fields|fieldsInfo as field %}
  {%if not field.ignore %}

NAN_GETTER({{ cppClassName }}::Get{{ field.cppFunctionName }}) {
  NanScope();

  {{ cppClassName }} *wrapper = ObjectWrap::Unwrap<{{ cppClassName }}>(args.This());

  {%if field.hasConstructor | or field.cppClassName == "Function" %}
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
  wrapper->{{ field.name }} = Persistent<Object>::New(value->ToObject());
  wrapper->raw->{{ field.name }} = *ObjectWrap::Unwrap<{{ field.cppClassName }}>(value->ToObject())->GetValue();
  {elsif field.cppClassName == "Function" %}
  wrapper->{{ field.name }} = Persistent<Function>::New(value->ToFunction());
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

    {%if field.cppClassName == "Function" %}
{{ field.returnType }} {{ field.name }}_cppCallback (
  {%each field.args|argsInfo as arg%}
  {{ arg.cType }} {{ arg.name}}{%if not arg.lastArg %},{%endif%}
  {%endeach}
  ) {
  // code to call JS function goes here
}
    {%endif%}
  {%endif%}
{%endeach%}

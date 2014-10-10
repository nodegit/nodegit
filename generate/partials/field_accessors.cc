{%each fields|fieldsInfo as field %}
  {%if not field.ignore %}

NAN_GETTER({{ cppClassName }}::Get{{ field.cppFunctionName }}) {
  NanScope();

  {{ cType }} *raw = ObjectWrap::Unwrap<{{ cppClassName }}>(args.This())->GetValue();

  {%if field.cppClassName == 'String' %}
  NanReturnValue(NanNew<String>(raw->{{ field.name }}));
  {%elsif field.cppClassName|isV8Value %}
  NanReturnValue(NanNew<{{ field.cppClassName }}>(raw->{{ field.name }}));
  {%endif%}

}

NAN_SETTER({{ cppClassName }}::Set{{ field.cppFunctionName }}) {
  NanScope();

  {{ cType }} *raw = ObjectWrap::Unwrap<{{ cppClassName }}>(args.This())->GetValue();

  {%if field.cppClassName == 'String' %}
  if (raw->{{ field.name }}) {
    free(raw->{{ field.name }});
  }

  String::Utf8Value str(value);
  raw->{{ field.name }} = strdup(*str);
  {%elsif field.isCppClassIntType%}
  if (value->IsNumber()) {
    raw->{{ field.name }} = value->field.cppClassName}}Value();
  }
  {%elsif field.cppClassName == 'Number' %}
  if (value->IsNumber()) {
    raw->{{ field.name }} = value->Int32Value();
  }
  {%else%}
  raw->{{ field.name }} = ({{ field.cType }}) value->ToObject();
  {%endif%}

}
  {%endif%}
{%endeach%}

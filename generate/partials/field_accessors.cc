{%each fields|fieldsInfo as field %}
  {%if not field.ignore %}

NAN_GETTER({{ cppClassName }}::Get{{ field.cppFunctionName }}) {
  NanScope();

  {{ cType }} *raw = ObjectWrap::Unwrap<{{ cppClassName }}>(args.This())->GetValue();

  {%if field.hasConstructor %}
  NanReturnValue(NanNew(new {{ field.cppClassName }}(&raw->{{ field.name }})));
  {%elsif field.cppClassName == 'String' %}
  NanReturnValue(NanNew<String>(raw->{{ field.name }}));
  {%elsif field.cppClassName|isV8Value %}
  NanReturnValue(NanNew<{{ field.cppClassName }}>(raw->{{ field.name }}));
  {%endif%}

}

NAN_SETTER({{ cppClassName }}::Set{{ field.cppFunctionName }}) {
  NanScope();

  {{ cType }} *raw = ObjectWrap::Unwrap<{{ cppClassName }}>(args.This())->GetValue();

  {%if field.hasConstructor %}
  {{ field.cType }}* wrappedStruct = ObjectWrap::Unwrap<{{ field.cppClassName }}>(value->ToObject())->GetValue();
  memcpy(&raw->{{ field.name }}, wrappedStruct, sizeof({{ field.cType }}));
  {%elsif field.cppClassName == 'String' %}
  if (raw->{{ field.name }}) {
    //free(raw->{{ field.name }});
  }

  String::Utf8Value str(value);
  raw->{{ field.name }} = strdup(*str);
  {%elsif field.isCppClassIntType%}
  if (value->IsNumber()) {
    raw->{{ field.name }} = value->{{field.cppClassName}}Value();
  }
  {%else%}
  if (value->IsNumber()) {
    raw->{{ field.name }} = ({{ field.cType }}) value->Int32Value();
  }
  {%endif%}

}
  {%endif%}
{%endeach%}

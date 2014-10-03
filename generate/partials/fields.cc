{%each fieldInfos as field %}
  {%if not field.ignore %}

NAN_METHOD({{ cppClassName }}::{{ field.cppFunctionName }}) {
  NanScope();
  Handle<Value> to;

  {{ field.cType }} {%if field.cppClassName|isV8Value %}*{%endif%}{{ field.name }} =
    {%if not field.cppClassName|isV8Value %}&{%endif%}ObjectWrap::Unwrap<{{ cppClassName }}>(args.This())->GetValue()->{{ field.name }};

  {%partial convertToV8 field %}
  NanReturnValue(to);
}
  {%endif%}
{%endeach%}

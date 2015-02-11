{% each fields|fieldsInfo as field %}
  {% if not field.ignore %}
    NAN_METHOD({{ cppClassName }}::{{ field.cppFunctionName }}) {
      NanScope();
      Handle<v8::Value> to;

      {{ field.cType }} {% if not field.cppClassName|isV8Value %}*{% endif %}{{ field.name }} =
        {% if not field.cppClassName|isV8Value %}&{% endif %}ObjectWrap::Unwrap<{{ cppClassName }}>(args.This())->GetValue()->{{ field.name }};

      {% partial convertToV8 field %}
      NanReturnValue(to);
    }
  {% endif %}
{% endeach %}

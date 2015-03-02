{% each fields|fieldsInfo as field %}
  {% if not field.ignore %}
    NAN_METHOD({{ cppClassName }}::{{ field.cppFunctionName }}) {
      NanScope();
      Handle<v8::Value> to;

      {% if field | isFixedLengthString %}
      char* {{ field.name }} = (char *)ObjectWrap::Unwrap<{{ cppClassName }}>(args.This())->GetValue()->{{ field.name }};
      {% else %}
      {{ field.cType }}
        {% if not field.cppClassName|isV8Value %}
          {% if not field.cType|isPointer %}
        *
          {% endif %}
        {% endif %}
        {{ field.name }} =
        {% if not field.cppClassName|isV8Value %}
          {% if not field.cType|isPointer %}
        &
          {% endif %}
        {% endif %}
        ObjectWrap::Unwrap<{{ cppClassName }}>(args.This())->GetValue()->{{ field.name }};
      {% endif %}

      {% partial convertToV8 field %}
      NanReturnValue(to);
    }
  {% endif %}
{% endeach %}

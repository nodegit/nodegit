{% each fields|fieldsInfo as field %}
  {% if not field.ignore %}
    // start field block
    NAN_METHOD({{ cppClassName }}::{{ field.cppFunctionName }}) {
      v8::Local<v8::Value> to;

      {% if field | isFixedLengthString %}
      char* {{ field.name }} = (char *)Nan::ObjectWrap::Unwrap<{{ cppClassName }}>(info.This())->GetValue()->{{ field.name }};
      {% else %}
        {% if field.cType|isArrayType %}
          {{ field.cType|arrayTypeToPlainType }} *{{ field.name }} =
        {% else %}
          {{ field.cType }}
          {% if not field.cppClassName|isV8Value %}
            {% if not field.cType|isPointer %}*{% endif %}
          {% endif %}
          {{ field.name }} =
          {% if not field.cppClassName|isV8Value %}
            {% if field.cType|isArrayType %}{% elsif not field.cType|isPointer %}
          &
            {% endif %}
          {% endif %}
        {% endif %}
        Nan::ObjectWrap::Unwrap<{{ cppClassName }}>(info.This())->GetValue()->{{ field.name }};
      {% endif %}

      {% partial convertToV8 field %}
      info.GetReturnValue().Set(to);
    }
    // end field block
  {% endif %}
{% endeach %}

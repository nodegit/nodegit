{% each fields|fieldsInfo as field %}
  {% if not field.ignore %}
    // start field block
    Napi::Value {{ cppClassName }}::{{ field.cppFunctionName }}(const Napi::CallbackInfo& info) {
      Napi::Env env = info.Env();
      Napi::Value v8ConversionSlot;

      {% if field | isFixedLengthString %}
      char* {{ field.name }} = (char *)NodeGitWrapper<{{ cppClassName }}Traits>::Unwrap<{{ cppClassName }}>(info.This().As<Napi::Object>())->GetValue()->{{ field.name }};
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
        NodeGitWrapper<{{ cppClassName }}Traits>::Unwrap<{{ cppClassName }}>(info.This().As<Napi::Object>())->GetValue()->{{ field.name }};
      {% endif %}

      {% partial convertToV8 field %}
      return v8ConversionSlot;
    }
    // end field block
  {% endif %}
{% endeach %}

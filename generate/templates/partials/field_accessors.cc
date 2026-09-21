{% each fields|fieldsInfo as field %}
  {% if not field.ignore %}
    Napi::Value {{ cppClassName }}::Get{{ field.cppFunctionName }}(const Napi::CallbackInfo& info) {

      {{ cppClassName }} *wrapper = NodeGitWrapper<{{ cppClassName }}Traits>::Unwrap<{{ cppClassName }}>(info.This().As<Napi::Object>());

      {% if field.isEnum %}
        return Napi::Number::New(info.Env(), (int)wrapper->GetValue()->{{ field.name }});

      {% elsif field.isLibgitType %}
        return wrapper->{{ field.name }}.Value();

      {% elsif field.cppClassName == 'String' %}
        if (wrapper->GetValue()->{{ field.name }}) {
          return Napi::String::New(info.Env(), wrapper->GetValue()->{{ field.name }});
        }
        else {
          return info.Env().Undefined();
        }

      {% elsif field.cppClassName|isV8Value %}
        return Napi::{{ field.cppClassName }}::New(info.Env(), wrapper->GetValue()->{{ field.name }});
      {% endif %}
    }

    void {{ cppClassName }}::Set{{ field.cppFunctionName }}(const Napi::CallbackInfo& info, const Napi::Value& value) {
      {{ cppClassName }} *wrapper = NodeGitWrapper<{{ cppClassName }}Traits>::Unwrap<{{ cppClassName }}>(info.This().As<Napi::Object>());

      {% if field.isEnum %}
        if (value.IsNumber()) {
          wrapper->GetValue()->{{ field.name }} = ({{ field.cType }}) value.ToNumber().Int32Value();
        }

      {% elsif field.isLibgitType %}
        Napi::Object {{ field.name }} = value.As<Napi::Object>();

        wrapper->{{ field.name }} = Napi::Persistent({{ field.name }});

        {% if field.cppClassName == 'GitStrarray' %}
          wrapper->raw->{{ field.name }} = {% if not field.cType | isPointer %}*{% endif %}StrArrayConverter::Convert({{ field.name }});
        {% else %}
          auto wrappedObject = NodeGitWrapper<{{ field.cppClassName }}Traits>::Unwrap<{{ field.cppClassName }}>({{ field.name }});
          wrapper->raw->{{ field.name }} = {% if not field.cType | isPointer %}*{% endif %}wrappedObject->GetValue();
          {%-- We are assuming that users are responsible enough to not replace fields on their structs mid-operation, and would rather build out code to prevent that than be smarter here --%}
          wrapper->AddReferenceCallbacks(
            {{ field.index }},
            [wrappedObject]() {
              wrappedObject->Reference();
            },
            [wrappedObject]() {
              wrappedObject->Unreference();
            }
          );
        {% endif %}

      {% elsif field.cppClassName == 'String' %}
        Napi::String str = value.As<Napi::String>();
        std::string strData = str.Utf8Value();
        wrapper->GetValue()->{{ field.name }} = strdup(strData.c_str());

      {% elsif field.isCppClassIntType %}
        if (value.IsNumber()) {
          wrapper->GetValue()->{{ field.name }} = value.{{ field.cppClassName }}Value();
        }

      {% else %}
        if (value.IsNumber()) {
          wrapper->GetValue()->{{ field.name }} = ({{ field.cType }}) value.ToNumber().Int32Value();
        }
      {% endif %}
    }
  {% endif %}
{% endeach %}

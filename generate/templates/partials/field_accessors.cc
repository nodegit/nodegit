{% each fields|fieldsInfo as field %}
  {% if not field.ignore %}
    NAN_GETTER({{ cppClassName }}::Get{{ field.cppFunctionName }}) {

      {{ cppClassName }} *wrapper = Nan::ObjectWrap::Unwrap<{{ cppClassName }}>(info.This());

      {% if field.isEnum %}
        info.GetReturnValue().Set(Nan::New((int)wrapper->GetValue()->{{ field.name }}));

      {% elsif field.isLibgitType %}
        info.GetReturnValue().Set(Nan::New(wrapper->{{ field.name }}));

      {% elsif field.cppClassName == 'String' %}
        if (wrapper->GetValue()->{{ field.name }}) {
          info.GetReturnValue().Set(Nan::New<String>(wrapper->GetValue()->{{ field.name }}).ToLocalChecked());
        }
        else {
          return;
        }

      {% elsif field.cppClassName|isV8Value %}
        info.GetReturnValue().Set(Nan::New<{{ field.cppClassName }}>(wrapper->GetValue()->{{ field.name }}));
      {% endif %}
    }

    NAN_SETTER({{ cppClassName }}::Set{{ field.cppFunctionName }}) {
      {{ cppClassName }} *wrapper = Nan::ObjectWrap::Unwrap<{{ cppClassName }}>(info.This());

      {% if field.isEnum %}
        if (value->IsNumber()) {
          wrapper->GetValue()->{{ field.name }} = ({{ field.cType }}) Nan::To<int32_t>(value).FromJust();
        }

      {% elsif field.isLibgitType %}
        v8::Local<Object> {{ field.name }}(Nan::To<v8::Object>(value).ToLocalChecked());

        wrapper->{{ field.name }}.Reset({{ field.name }});

        {% if field.cppClassName == 'GitStrarray' %}
          wrapper->raw->{{ field.name }} = {% if not field.cType | isPointer %}*{% endif %}StrArrayConverter::Convert({{ field.name }});
        {% else %}
          auto wrappedObject = Nan::ObjectWrap::Unwrap<{{ field.cppClassName }}>({{ field.name }});
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
        if (wrapper->GetValue()->{{ field.name }}) {
        }

        Nan::Utf8String str(value);
        wrapper->GetValue()->{{ field.name }} = strdup(*str);

      {% elsif field.isCppClassIntType %}
        if (value->IsNumber()) {
          wrapper->GetValue()->{{ field.name }} = value->{{field.cppClassName}}Value();
        }

      {% else %}
        if (value->IsNumber()) {
          wrapper->GetValue()->{{ field.name }} = ({{ field.cType }}) Nan::To<int32_t>(value).FromJust();
        }
      {% endif %}
    }
  {% endif %}
{% endeach %}

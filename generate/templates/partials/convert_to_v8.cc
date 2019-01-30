// start convert_to_v8 block
{% if cppClassName == 'String' %}
  if ({{= parsedName =}}){
    {% if size %}
      to = Nan::New<v8::String>({{= parsedName =}}, {{ size }}).ToLocalChecked();
    {% elsif cType == 'char **' %}
      to = Nan::New<v8::String>(*{{= parsedName =}}).ToLocalChecked();
    {% elsif cType == 'char' %}
      char convertToNullTerminated[2] = { {{= parsedName =}}, '\0' };
      to = Nan::New<v8::String>(convertToNullTerminated).ToLocalChecked();
    {% else %}
      to = Nan::New<v8::String>({{= parsedName =}}).ToLocalChecked();
    {% endif %}
  }
  else {
    to = Nan::Null();
  }

  {% if freeFunctionName %}
    {{ freeFunctionName }}({{= parsedName =}});
  {% endif %}

{% elsif cppClassName|isV8Value %}
  {% if cType|isArrayType %}
    v8::Local<Array> tmpArray = Nan::New<Array>({{ cType|toSizeOfArray }});
    for (unsigned int i = 0; i < {{ cType|toSizeOfArray }}; i++) {
      v8::Local<v8::Value> element;
      {% if isCppClassIntType %}
        element = Nan::New<{{ cppClassName }}>(({{ parsedClassName }}){{= parsedName =}}[i]);
      {% else %}
      element = Nan::New<{{ cppClassName }}>({% if needsDereference %}*{% endif %}{{= parsedName =}}[i]);
      {% endif %}
      Nan::Set(tmpArray, Nan::New<Number>(i), element);
    }
    to = tmpArray;
  {% elsif isCppClassIntType %}
    to = Nan::New<{{ cppClassName }}>(({{ parsedClassName }}){{= parsedName =}});
  {% else %}
    to = Nan::New<{{ cppClassName }}>({% if needsDereference %}*{% endif %}{{= parsedName =}});
  {% endif %}

{% elsif cppClassName == 'External' %}

  to = Nan::New<External>((void *){{= parsedName =}});

{% elsif cppClassName == 'Array' %}

  {%-- // FIXME this is not general purpose enough. --%}
  {% if size %}
    v8::Local<Array> tmpArray = Nan::New<Array>({{= parsedName =}}->{{ size }});
    for (unsigned int i = 0; i < {{= parsedName =}}->{{ size }}; i++) {
      Nan::Set(tmpArray, Nan::New<Number>(i), Nan::New<v8::String>({{= parsedName =}}->{{ key }}[i]).ToLocalChecked());
    }
  {% else %}
    v8::Local<Array> tmpArray = Nan::New<Array>({{= parsedName =}});
  {% endif %}

  to = tmpArray;
{% elsif cppClassName == 'GitBuf' %}
  {% if doNotConvert %}
  to = Nan::Null();
  {% else %}
  if ({{= parsedName =}}) {
    to = Nan::New<v8::String>({{= parsedName =}}->ptr, {{= parsedName = }}->size).ToLocalChecked();
  }
  else {
    to = Nan::Null();
  }
  {% endif %}
{% else %}
  {% if cType|isArrayType %}
    v8::Local<Array> tmpArray = Nan::New<Array>({{ cType|toSizeOfArray }});
    for (unsigned int i = 0; i < {{ cType|toSizeOfArray }}; i++) {
  {% endif %}
  if ({{ cType|asElementPointer parsedName }} != NULL) {
    {% if hasOwner %}
      v8::Local<v8::Array> owners = Nan::New<Array>(0);
      {% if ownedBy %}
        {% if isAsync %}
          {% each ownedBy as owner %}
            {%-- If the owner of this object is "this" in an async method, it will be stored in the persistent handle by name. --%}
            Nan::Set(owners, Nan::New<v8::Number>(owners->Length()), this->GetFromPersistent("{{= owner =}}")->ToObject());
          {% endeach %}
        {% else %}
          {% each ownedByIndices as ownedByIndex %}
            Nan::Set(owners, Nan::New<v8::Number>(owners->Length()), info[{{= ownedByIndex =}}]->ToObject());
          {% endeach %}
        {% endif %}
      {% endif %}
      {%if isAsync %}
      {% elsif ownedByThis %}
        {%-- If the owner of this object is "this", it will be retrievable from the info object in a sync method. --%}
        Nan::Set(owners, owners->Length(), info.This());
      {% endif %}
      {% if ownerFn | toBool %}
        Nan::Set(
          owners,
          Nan::New<v8::Number>(owners->Length()),
          {{= ownerFn.singletonCppClassName =}}::New(
            {{= ownerFn.name =}}({{ cType|asElementPointer parsedName }}),
            true
          )->ToObject()
        );
      {% endif %}
    {% endif %}
    {% if cppClassName == 'Wrapper' %}
      to = {{ cppClassName }}::New({{ cType|asElementPointer parsedName }});
    {% else %}
      to = {{ cppClassName }}::New(
        {{ cType|asElementPointer parsedName }},
        {{ selfFreeing|toBool }}
        {% if hasOwner %}
          , owners
        {% endif %}
      );
    {% endif %}
  }
  else {
    to = Nan::Null();
  }
  {% if cType|isArrayType %}
      Nan::Set(tmpArray, Nan::New<Number>(i), to);
    }
    to = tmpArray;
  {% endif %}
{% endif %}
// end convert_to_v8 block

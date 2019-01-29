// start convert_to_v8 block
{% if cppClassName == 'String' %}
  if ({{= parsedName =}}){
    {% if size %}
      to = Nan::New<v8::String>({{= parsedName =}}, {{ size }}).ToLocalChecked();
    {% elsif cType == 'char **' %}
      to = Nan::New<v8::String>(*{{= parsedName =}}).ToLocalChecked();
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
{% elsif cType|isArrayType %}
  v8::Local<Array> tmpArray = Nan::New<Array>({{ cType|toSizeOfArray }});
  for (unsigned int i = 0; i < {{ cType|toSizeOfArray }}; i++) {
    v8::Local<v8::Value> element;
    {{ cType|arrayTypeToPlainType }} *rawElement = &{{= parsedName =}}[i];

    {% if copy %}
      if (rawElement != NULL) {
        rawElement = {{ copy }}(rawElement);
      }
    {% endif %}

    if (rawElement != NULL) {
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
              {{= ownerFn.name =}}(rawElement),
              true
            )->ToObject()
          );
        {% endif %}
      {% endif %}
      {% if cppClassName == 'Wrapper' %}
        element = {{ cppClassName }}::New(rawElement);
      {% else %}
        element = {{ cppClassName }}::New(
          rawElement,
          {{ selfFreeing|toBool }}
          {% if hasOwner %}
            , owners
          {% endif %}
        );
      {% endif %}
    }
    else {
      element = Nan::Null();
    }

    Nan::Set(tmpArray, Nan::New<Number>(i), element);
  }
  to = tmpArray;
{% else %}
  {% if copy %}
    if ({{= parsedName =}} != NULL) {
      {{= parsedName =}} = ({{ cType|replace '**' '*' }} {% if not cType|isPointer %}*{% endif %}){{ copy }}({{= parsedName =}});
    }
  {% endif %}

  if ({{= parsedName =}} != NULL) {
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
            {{= ownerFn.name =}}({{= parsedName =}}),
            true
          )->ToObject()
        );
      {% endif %}
    {% endif %}
    {% if cppClassName == 'Wrapper' %}
      to = {{ cppClassName }}::New({{= parsedName =}});
    {% else %}
      to = {{ cppClassName }}::New(
        {{= parsedName =}},
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

{% endif %}
// end convert_to_v8 block

// start convert_to_v8 block
// NOTE: relies on a Napi::Env `env` in scope (defined by the caller).
{% if cppClassName == 'String' %}
  if ({{= parsedName =}}){
    {% if size %}
      v8ConversionSlot = Napi::String::New(env, {{= parsedName =}}, {{ size }});
    {% elsif cType == 'char **' %}
      v8ConversionSlot = Napi::String::New(env, *{{= parsedName =}});
    {% elsif cType == 'char' %}
      char convertToNullTerminated[2] = { {{= parsedName =}}, '\0' };
      v8ConversionSlot = Napi::String::New(env, convertToNullTerminated);
    {% else %}
      v8ConversionSlot = Napi::String::New(env, {{= parsedName =}});
    {% endif %}
  }
  else {
    v8ConversionSlot = env.Null();
  }

  {% if freeFunctionName %}
    {{ freeFunctionName }}({{= parsedName =}});
  {% endif %}

{% elsif cppClassName|isV8Value %}
  {% if cType|isArrayType %}
    Napi::Array tmpArray = Napi::Array::New(env, {{ cType|toSizeOfArray }});
    for (unsigned int i = 0; i < {{ cType|toSizeOfArray }}; i++) {
      Napi::Value element;
      {% if isCppClassIntType %}
        element = Napi::Number::New(env, ({{ parsedClassName }}){{= parsedName =}}[i]);
      {% else %}
        element = Napi::Number::New(env, ({% if needsDereference %}*{% endif %}{{= parsedName =}}[i]));
      {% endif %}
      tmpArray.Set(i, element);
    }
    v8ConversionSlot = tmpArray;
  {% elsif isCppClassIntType %}
    v8ConversionSlot = Napi::Number::New(env, ({{ parsedClassName }}){{= parsedName =}});
  {% else %}
    v8ConversionSlot = Napi::{{ cppClassName }}::New(env, {% if needsDereference %}*{% endif %}{{= parsedName =}});
  {% endif %}

{% elsif cppClassName == 'External' %}

  v8ConversionSlot = Napi::External<void>::New(env, (void *){{= parsedName =}});

{% elsif cppClassName == 'Array' %}

  {%-- // FIXME this is not general purpose enough. --%}
  {% if size %}
    Napi::Array tmpArray = Napi::Array::New(env, {{= parsedName =}}->{{ size }});
    for (unsigned int i = 0; i < {{= parsedName =}}->{{ size }}; i++) {
      Napi::Value element;
      {% if arrayElementCppClassName %}
        element = {{ arrayElementCppClassName }}::New(
          {{ cType|asElementPointer parsedName }}->{{ key }}[i],
          {{ selfFreeing|toBool }}
          {% if hasOwner %}
            , owners
          {% endif %}
        );
      {% else %}
        element = Napi::String::New(env, {{= parsedName =}}->{{ key }}[i]);
      {% endif %}
      tmpArray.Set(i, element);
    }
  {% else %}
    Napi::Array tmpArray = Napi::Array::New(env, {{= parsedName =}});
  {% endif %}

  v8ConversionSlot = tmpArray;
{% elsif cppClassName == 'GitBuf' %}
  {% if doNotConvert %}
  v8ConversionSlot = env.Null();
  {% else %}
  if ({{= parsedName =}}) {
    v8ConversionSlot = Napi::String::New(env, {{= parsedName =}}->ptr, {{= parsedName =}}->size);
  }
  else {
    v8ConversionSlot = env.Null();
  }
  {% endif %}
{% else %}
  {% if cType|isArrayType %}
    Napi::Array tmpArray = Napi::Array::New(env, {{ cType|toSizeOfArray }});
    for (unsigned int i = 0; i < {{ cType|toSizeOfArray }}; i++) {
  {% endif %}
  if ({{ cType|asElementPointer parsedName }} != NULL) {
    {% if hasOwner | or ownedByThis %}
      Napi::Array owners = Napi::Array::New(env, 0);
      {% if ownedBy %}
        {% if isAsync %}
          {% each ownedBy as owner %}
            {%-- If the owner of this object is "this" in an async method, it will be stored in the persistent handle by name. --%}
            owners.Set(owners.Length(), this->GetFromPersistent("{{= owner =}}").As<Napi::Value>());
          {% endeach %}
        {% else %}
          {% each ownedByIndices as ownedByIndex %}
            owners.Set(owners.Length(), info[{{= ownedByIndex =}}].As<Napi::Value>());
          {% endeach %}
        {% endif %}
      {% endif %}
      {%if isAsync %}
      {% elsif ownedByThis %}
        {%-- If the owner of this object is "this", it will be retrievable from the info object in a sync method. --%}
        owners.Set(owners.Length(), info.This().As<Napi::Value>());
      {% endif %}
      {% if ownerFn | toBool %}
        owners.Set(
          owners.Length(),
          {{= ownerFn.singletonCppClassName =}}::New(
            {{= ownerFn.name =}}({{ cType|asElementPointer parsedName }}),
            false
          ).As<Napi::Value>()
        );
      {% endif %}
    {% endif %}
    {% if cppClassName == 'Wrapper' %}
      v8ConversionSlot = {{ cppClassName }}::New(env, {{ cType|asElementPointer parsedName }});
    {% else %}
      v8ConversionSlot = {{ cppClassName }}::New(
        {{ cType|asElementPointer parsedName }},
        {{ selfFreeing|toBool }}
        {% if hasOwner | or ownedByThis %}
          , owners
        {% endif %}
      );
    {% endif %}
  }
  else {
    v8ConversionSlot = env.Null();
  }
  {% if cType|isArrayType %}
      tmpArray.Set(i, v8ConversionSlot);
    }
    v8ConversionSlot = tmpArray;
  {% endif %}
{% endif %}
// end convert_to_v8 block

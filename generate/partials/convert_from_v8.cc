{%if not isPayload %}
  {{ cType }} from_{{ name }};
  {%if isOptional %}

  if (args[{{ jsArg }}]->Is{{ cppClassName|cppToV8 }}()) {
  {%endif%}
  {%if cppClassName == 'String' %}

  String::Utf8Value {{ name }}(args[{{ jsArg }}]->ToString());
  from_{{ name }} = ({{ cType }}) strdup(*{{ name }});
  {%elsif cppClassName == 'Array' %}

  Array *tmp_{{ name }} = Array::Cast(*args[{{ jsArg }}]);
  from_{{ name }} = ({{ cType }})malloc(tmp_{{ name }}->Length() * sizeof({{ cType|replace '**' '*' }}));
      for (unsigned int i = 0; i < tmp_{{ name }}->Length(); i++) {
    {%--
      // FIXME: should recursively call convertFromv8.
    --%}
      from_{{ name }}[i] = ObjectWrap::Unwrap<{{ arrayElementCppClassName }}>(tmp_{{ name }}->Get(NanNew<Number>(static_cast<double>(i)))->ToObject())->GetValue();
      }
  {%elsif cppClassName == 'Function' %}
  {%elsif cppClassName == 'Buffer' %}

  from_{{ name }} = Buffer::Data(args[{{ jsArg }}]->ToObject());
  {%elsif cppClassName|isV8Value %}

  from_{{ name }} = ({{ cType }}) {{ additionalCast }} {{ cast }} args[{{ jsArg }}]->To{{ cppClassName }}()->Value();
  {%else%}
    from_{{ name }} = ObjectWrap::Unwrap<{{ cppClassName }}>(args[{{ jsArg }}]->ToObject())->GetValue();
  {%endif%}

  {%if isOptional %}
  }
  else {
    from_{{ name }} = 0;
  }

  {%endif%}
{%endif%}

{%if not arg.isPayload %}
  {{ arg.cType }} from_{{ arg.name }};
  {%if arg.isOptional %}
    if (args{{[ arg.jsArg }}]->Is{{ arg.v8ValueClassName }}()) {
  {%endif%}
  {%if arg.cppClassName == 'String' %}
      String::Utf8Value {{ arg.name }}(args[{{ arg.jsArg }}]->ToString());
      from_{{ arg.name }} = ({{ arg.cType }}) strdup(*{{ arg.name }});
  {%elsif arg.cppClassName == 'Array' %}
      Array *tmp_{{ arg.name }} = Array::Cast(*args[{{ arg.jsArg }}]);
      from_{{ arg.name }} = ({{ arg.cType }})malloc(tmp_{{ arg.name }}->Length() * sizeof({{ arg.cType|replace '**' '*' }}));
      for (unsigned int i = 0; i < tmp_{{ arg.name }}->Length(); i++) {
    {%--
      // FIXME: should recursively call convertFromv8.
    --%}
        from_{{ arg.name }}[i] = ObjectWrap::Unwrap<{{ arg.arrayElementCppClassName }}>(tmp_{{ arg.name }}->Get(NanNew<Number>(static_cast<double>(i)))->ToObject())->GetValue();
      }
  {%elsif arg.cppClassName == 'Function' %}
  {%elsif arg.cppClassName == 'Buffer' %}
      from_{{ arg.name }} = Buffer::Data(args[{{ arg.jsArg }}]->ToObject());
  {%elsif arg.cppClassName|isV8Value %}
      from_{{ arg.name }} = ({{ arg.cType }}) {{ arg.additionalCast }} {{ arg.cast }} args[{{ arg.jsArg }}]->To{{ arg.cppClassName }}()->Value();
  {%else%}
      from_{{ arg.name }} = ObjectWrap::Unwrap<{{ arg.cppClassName }}>(args[{{ arg.jsArg }}]->ToObject())->GetValue();
  {%endif%}

  {%if arg.isOptional %}
    } else {
      from_{{ arg.name }} = 0;
    }
  {%endif%}}
{%endif%}

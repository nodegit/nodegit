{%if not isPayload %}
// start convert_from_v8 block
  {{ cType }} from_{{ name }};
  {%if isOptional | or isBoolean %}

    {%if cppClassName != 'GitStrarray'%}
  if (args[{{ jsArg }}]->Is{{ cppClassName|cppToV8 }}()) {
    {%endif%}
  {%endif%}
  {%if cppClassName == 'String'%}

  String::Utf8Value {{ name }}(args[{{ jsArg }}]->ToString());
  from_{{ name }} = ({{ cType }}) strdup(*{{ name }});
  {%elsif cppClassName == 'GitStrarray' %}

  from_{{ name }} = StrArrayConverter::Convert(args[{{ jsArg }}]);
  {%elsif cppClassName == 'Wrapper'%}

  String::Utf8Value {{ name }}(args[{{ jsArg }}]->ToString());
  from_{{ name }} = ({{ cType }}) strdup(*{{ name }});
  {%elsif cppClassName == 'Array'%}

  Array *tmp_{{ name }} = Array::Cast(*args[{{ jsArg }}]);
  from_{{ name }} = ({{ cType }})malloc(tmp_{{ name }}->Length() * sizeof({{ cType|replace '**' '*' }}));
      for (unsigned int i = 0; i < tmp_{{ name }}->Length(); i++) {
    {%--
      // FIXME: should recursively call convertFromv8.
    --%}
      from_{{ name }}[i] = ObjectWrap::Unwrap<{{ arrayElementCppClassName }}>(tmp_{{ name }}->Get(NanNew<Number>(static_cast<double>(i)))->ToObject())->GetValue();
      }
  {%elsif cppClassName == 'Function'%}
  {%elsif cppClassName == 'Buffer'%}

  from_{{ name }} = Buffer::Data(args[{{ jsArg }}]->ToObject());
  {%elsif cppClassName|isV8Value %}

    {%if cType|isPointer %}
  *from_{{ name }} = ({{ cType|unPointer }}) {{ cast }} {%if isEnum %}(int){%endif%} args[{{ jsArg }}]->To{{ cppClassName }}()->Value();
    {%else%}
  from_{{ name }} = ({{ cType }}) {{ cast }} {%if isEnum %}(int){%endif%} args[{{ jsArg }}]->To{{ cppClassName }}()->Value();
    {%endif%}
  {%elsif cppClassName == 'GitOid'%}
  if (args[{{ jsArg }}]->IsString()) {
    // Try and parse in a string to a git_oid
    String::Utf8Value oidString(args[{{ jsArg }}]->ToString());
    git_oid *oidOut = (git_oid *)malloc(sizeof(git_oid));

    if (git_oid_fromstr(oidOut, (const char *) strdup(*oidString)) != GIT_OK) {
      free(oidOut);

      if (giterr_last()) {
        return NanThrowError(giterr_last()->message);
      } else {
        return NanThrowError("Unknown Error");
      }
    }

    {%if cType|isDoublePointer %}
    from_{{ name }} = &oidOut;
    {%else%}
    from_{{ name }} = oidOut;
    {%endif%}
  }
  else {
    {%if cType|isDoublePointer %}
    from_{{ name }} = ObjectWrap::Unwrap<{{ cppClassName }}>(args[{{ jsArg }}]->ToObject())->GetRefValue();
    {%else%}
    from_{{ name }} = ObjectWrap::Unwrap<{{ cppClassName }}>(args[{{ jsArg }}]->ToObject())->GetValue();
    {%endif%}
  }
  {%else%}
    {%if cType|isDoublePointer %}
  from_{{ name }} = ObjectWrap::Unwrap<{{ cppClassName }}>(args[{{ jsArg }}]->ToObject())->GetRefValue();
    {%else%}
  from_{{ name }} = ObjectWrap::Unwrap<{{ cppClassName }}>(args[{{ jsArg }}]->ToObject())->GetValue();
    {%endif%}
  {%endif%}

  {%if isBoolean %}
  }
  else {
    from_{{ name }} = args[{{ jsArg }}]->IsTrue() ? 1 : 0;
  }
  {%elsif isOptional %}
    {%if cppClassName != 'GitStrarray'%}
  }
  else {
    from_{{ name }} = 0;
  }
    {%endif%}
  {%endif%}
// end convert_from_v8 block
{%endif%}

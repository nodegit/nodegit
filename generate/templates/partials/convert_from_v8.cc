{%if not isPayload %}
// start convert_from_v8 block
  {%if cType|isPointer %}
  {{ cType }} from_{{ name }} = NULL;
  {%elsif cType|isDoublePointer %}
  {{ cType }} from_{{ name }} = NULL;
  {%else%}
  {{ cType }} from_{{ name }};
  {%endif%}
  {%if isOptional | or isBoolean %}

    {%if cppClassName == 'GitStrarray'%}
    {%-- Print nothing --%}
    {% elsif cppClassName == 'GitBuf' %}
    {%-- Print nothing --%}
    {%else%}
    if ((info.Length() - 1) > {{ jsArg }} && info[{{ jsArg }}]->Is{{ cppClassName|cppToV8 }}()) {
      {%endif%}
    {%endif%}
  {%if cppClassName == 'String'%}

  Nan::Utf8String {{ name }}(Nan::To<v8::String>(info[{{ jsArg }}]).ToLocalChecked());
  // malloc with one extra byte so we can add the terminating null character C-strings expect:
  from_{{ name }} = ({{ cType }}) malloc({{ name }}.length() + 1);
  // copy the characters from the nodejs string into our C-string (used instead of strdup or strcpy because nulls in
  // the middle of strings are valid coming from nodejs):
  memcpy((void *)from_{{ name }}, *{{ name }}, {{ name }}.length());
  // ensure the final byte of our new string is null, extra casts added to ensure compatibility with various C types
  // used in the nodejs binding generation:
  memset((void *)(((char *)from_{{ name }}) + {{ name }}.length()), 0, 1);
  {%elsif cppClassName == 'GitStrarray' %}

  from_{{ name }} = StrArrayConverter::Convert(info[{{ jsArg }}]);
  {%elsif  cppClassName == 'GitBuf' %}

  from_{{ name }} = GitBufConverter::Convert(info[{{ jsArg }}]);
  {%elsif cppClassName == 'Wrapper'%}

  Nan::Utf8String {{ name }}(Nan::To<v8::String>(info[{{ jsArg }}]).ToLocalChecked());
  // malloc with one extra byte so we can add the terminating null character C-strings expect:
  from_{{ name }} = ({{ cType }}) malloc({{ name }}.length() + 1);
  // copy the characters from the nodejs string into our C-string (used instead of strdup or strcpy because nulls in
  // the middle of strings are valid coming from nodejs):
  memcpy((void *)from_{{ name }}, *{{ name }}, {{ name }}.length());
  // ensure the final byte of our new string is null, extra casts added to ensure compatibility with various C types
  // used in the nodejs binding generation:
  memset((void *)(((char *)from_{{ name }}) + {{ name }}.length()), 0, 1);
  {%elsif cppClassName == 'Array'%}

  v8::Local<v8::Array> tmp_{{ name }} = v8::Local<v8::Array>::Cast(info[{{ jsArg }}]);
  from_{{ name }} = ({{ cType }})malloc(tmp_{{ name }}->Length() * sizeof({{ cType|unPointer }}));
  for (unsigned int i = 0; i < tmp_{{ name }}->Length(); i++) {
    {%--
      // FIXME: should recursively call convertFromv8.
    --%}
    const v8::Local<v8::Value> arrayVal = Nan::Get(tmp_{{ name }},i).ToLocalChecked();
    {%if arrayElementCppClassName == 'GitOid'%}
      if (arrayVal->IsString()) {
        // Try and parse in a string to a git_oid
        Nan::Utf8String oidString(Nan::To<v8::String>(arrayVal).ToLocalChecked());

        if (git_oid_fromstr(&from_{{ name }}[i], (const char *) strdup(*oidString)) != GIT_OK) {
          if (git_error_last()) {
            return Nan::ThrowError(git_error_last()->message);
          } else {
            return Nan::ThrowError("Unknown Error");
          }
        }
      }
      else {
        git_oid_cpy(&from_{{ name }}[i], Nan::ObjectWrap::Unwrap<GitOid>(Nan::To<v8::Object>(arrayVal).ToLocalChecked())->GetValue());
      }
    {%else%}
      from_{{ name }}[i] = Nan::ObjectWrap::Unwrap<{{ arrayElementCppClassName }}>(Nan::To<v8::Object>(arrayVal).ToLocalChecked())->GetValue();
    {%endif%}
  }
  {%elsif cppClassName == 'Function'%}
  {%elsif cppClassName == 'Buffer'%}

  from_{{ name }} = Buffer::Data(Nan::To<v8::Object>(info[{{ jsArg }}]).ToLocalChecked());
  {%elsif cppClassName|isV8Value %}

    {%if cType|isPointer %}
      *from_{{ name }} = ({{ cType|unPointer }}) {{ cast }} {%if isEnum %}(int){%endif%} info[{{ jsArg }}].As<v8::{{ cppClassName }}>()->Value();
    {%else%}
      from_{{ name }} = ({{ cType }}) {{ cast }} {%if isEnum %}(int){%endif%} info[{{ jsArg }}].As<v8::{{ cppClassName }}>()->Value();
    {%endif%}
  {%elsif cppClassName == 'GitOid'%}
  if (info[{{ jsArg }}]->IsString()) {
    // Try and parse in a string to a git_oid
    Nan::Utf8String oidString(Nan::To<v8::String>(info[{{ jsArg }}]).ToLocalChecked());
    git_oid *oidOut = (git_oid *)malloc(sizeof(git_oid));

    if (git_oid_fromstr(oidOut, (const char *) strdup(*oidString)) != GIT_OK) {
      free(oidOut);

      if (git_error_last()) {
        return Nan::ThrowError(git_error_last()->message);
      } else {
        return Nan::ThrowError("Unknown Error");
      }
    }

    {%if cType|isDoublePointer %}
    from_{{ name }} = &oidOut;
    {%else%}
    from_{{ name }} = oidOut;
    {%endif%}
  }
  else {
    {%if cType|isDoublePointer %}*{%endif%}from_{{ name }} = Nan::ObjectWrap::Unwrap<{{ cppClassName }}>(Nan::To<v8::Object>(info[{{ jsArg }}]).ToLocalChecked())->GetValue();
  }
  {%else%}
    {%if cType|isDoublePointer %}*{%endif%}from_{{ name }} = Nan::ObjectWrap::Unwrap<{{ cppClassName }}>(Nan::To<v8::Object>(info[{{ jsArg }}]).ToLocalChecked())->GetValue();
  {%endif%}

  {%if isBoolean %}
  }
  else {
    from_{{ name }} = info[{{ jsArg }}]->IsTrue() ? 1 : 0;
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

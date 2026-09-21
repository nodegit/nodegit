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
    if ((info.Length() - 1) > {{ jsArg }} && info[{{ jsArg }}].Is{{ cppClassName|cppToV8 }}()) {
      {%endif%}
    {%endif%}
  {%if cppClassName == 'String'%}

  Napi::String {{ name }}Str = info[{{ jsArg }}].As<Napi::String>();
  std::string {{ name }}Utf8 = {{ name }}Str.Utf8Value();
  // malloc with one extra byte so we can add the terminating null character C-strings expect:
  from_{{ name }} = ({{ cType }}) malloc({{ name }}Utf8.length() + 1);
  // copy the characters from the nodejs string into our C-string (used instead of strdup or strcpy because nulls in
  // the middle of strings are valid coming from nodejs):
  memcpy((void *)from_{{ name }}, {{ name }}Utf8.c_str(), {{ name }}Utf8.length());
  // ensure the final byte of our new string is null, extra casts added to ensure compatibility with various C types
  // used in the nodejs binding generation:
  memset((void *)(((char *)from_{{ name }}) + {{ name }}Utf8.length()), 0, 1);
  {%elsif cppClassName == 'GitStrarray' %}

  from_{{ name }} = StrArrayConverter::Convert(info[{{ jsArg }}]);
  {%elsif  cppClassName == 'GitBuf' %}

  from_{{ name }} = GitBufConverter::Convert(info[{{ jsArg }}]);
  {%elsif cppClassName == 'Wrapper'%}

  Napi::String {{ name }}Str = info[{{ jsArg }}].As<Napi::String>();
  std::string {{ name }}Utf8 = {{ name }}Str.Utf8Value();
  // malloc with one extra byte so we can add the terminating null character C-strings expect:
  from_{{ name }} = ({{ cType }}) malloc({{ name }}Utf8.length() + 1);
  // copy the characters from the nodejs string into our C-string (used instead of strdup or strcpy because nulls in
  // the middle of strings are valid coming from nodejs):
  memcpy((void *)from_{{ name }}, {{ name }}Utf8.c_str(), {{ name }}Utf8.length());
  // ensure the final byte of our new string is null, extra casts added to ensure compatibility with various C types
  // used in the nodejs binding generation:
  memset((void *)(((char *)from_{{ name }}) + {{ name }}Utf8.length()), 0, 1);
  {%elsif cppClassName == 'Array'%}

  Napi::Array tmp_{{ name }} = info[{{ jsArg }}].As<Napi::Array>();
  from_{{ name }} = ({{ cType }})malloc(tmp_{{ name }}.Length() * sizeof({{ cType|unPointer }}));
  for (unsigned int i = 0; i < tmp_{{ name }}.Length(); i++) {
    {%--
      // FIXME: should recursively call convertFromv8.
    --%}
    const Napi::Value arrayVal = tmp_{{ name }}.Get(i);
    {%if arrayElementCppClassName == 'GitOid'%}
      if (arrayVal.IsString()) {
        // Try and parse in a string to a git_oid
        Napi::String oidStringStr = arrayVal.As<Napi::String>();
        std::string oidString = oidStringStr.Utf8Value();

        if (git_oid_fromstr(&from_{{ name }}[i], (const char *) strdup(oidString.c_str())) != GIT_OK) {
          Napi::Error::New(env, git_error_last()->message).ThrowAsJavaScriptException();
          return env.Undefined();
        }
      }
      else {
        git_oid_cpy(&from_{{ name }}[i], NodeGitWrapper<GitOidTraits>::Unwrap<GitOid>(arrayVal.As<Napi::Object>())->GetValue());
      }
    {%else%}
      from_{{ name }}[i] = NodeGitWrapper<{{ arrayElementCppClassName }}Traits>::Unwrap<{{ arrayElementCppClassName }}>(arrayVal.As<Napi::Object>())->GetValue();
    {%endif%}
  }
  {%elsif cppClassName == 'Function'%}
  {%elsif cppClassName == 'Buffer'%}

  from_{{ name }} = info[{{ jsArg }}].As<Napi::Buffer<char>>().Data();
  {%elsif cppClassName|isV8Value %}

    {%if cppClassName == 'Boolean' %}
      {%if cType|isPointer %}
        *from_{{ name }} = ({{ cType|unPointer }}) {{ cast }} {%if isEnum %}(int){%endif%} info[{{ jsArg }}].As<Napi::Boolean>().Value();
      {%else%}
        from_{{ name }} = ({{ cType }}) {{ cast }} {%if isEnum %}(int){%endif%} info[{{ jsArg }}].As<Napi::Boolean>().Value();
      {%endif%}
    {%else%}
      {%if cType|isPointer %}
        *from_{{ name }} = ({{ cType|unPointer }}) {{ cast }} {%if isEnum %}(int){%endif%} info[{{ jsArg }}].As<Napi::Number>().DoubleValue();
      {%else%}
        from_{{ name }} = ({{ cType }}) {{ cast }} {%if isEnum %}(int){%endif%} info[{{ jsArg }}].As<Napi::Number>().DoubleValue();
      {%endif%}
    {%endif%}
  {%elsif cppClassName == 'GitOid'%}
  if (info[{{ jsArg }}].IsString()) {
    // Try and parse in a string to a git_oid
    Napi::String oidStringStr = info[{{ jsArg }}].As<Napi::String>();
    std::string oidString = oidStringStr.Utf8Value();
    git_oid *oidOut = (git_oid *)malloc(sizeof(git_oid));

    if (git_oid_fromstr(oidOut, (const char *) strdup(oidString.c_str())) != GIT_OK) {
      free(oidOut);

      if (git_error_last()->klass != GIT_ERROR_NONE) {
        Napi::Error::New(env, git_error_last()->message).ThrowAsJavaScriptException();
        return env.Undefined();
      } else {
        Napi::Error::New(env, "Unknown Error").ThrowAsJavaScriptException();
        return env.Undefined();
      }
    }

    {%if cType|isDoublePointer %}
    from_{{ name }} = &oidOut;
    {%else%}
    from_{{ name }} = oidOut;
    {%endif%}
  }
  else {
    {%if cType|isDoublePointer %}*{%endif%}from_{{ name }} = NodeGitWrapper<{{ cppClassName }}Traits>::Unwrap<{{ cppClassName }}>(info[{{ jsArg }}].As<Napi::Object>())->GetValue();
  }
  {%else%}
    {%if cType|isDoublePointer %}*{%endif%}from_{{ name }} = NodeGitWrapper<{{ cppClassName }}Traits>::Unwrap<{{ cppClassName }}>(info[{{ jsArg }}].As<Napi::Object>())->GetValue();
  {%endif%}

  {%if isBoolean %}
  }
  else {
    from_{{ name }} = info[{{ jsArg }}].IsTrue() ? 1 : 0;
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


{%each args|argsInfo as arg%}
  {%if arg.isJsArg%}
    {%if not arg.isOptional%}
      {%if not arg.payloadFor %}
        {%if arg | isOid %}
  if (info.Length() == {{arg.jsArg}}
    || (!info[{{arg.jsArg}}].IsObject() && !info[{{arg.jsArg}}].IsString())) {
    Napi::Error::New(info.Env(), "{{arg.jsClassName}} {{arg.name}} is required.").ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }
        {%elsif arg.isCallbackFunction %}
  if (info.Length() == {{arg.jsArg}} || !info[{{arg.jsArg}}].IsFunction()) {
    Napi::Error::New(info.Env(), "{{arg.jsClassName}} {{arg.name}} is required.").ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }
        {%elsif arg.cppClassName == "GitStrarray" %}
  if (info.Length() == {{arg.jsArg}} || (!info[{{arg.jsArg}}].IsArray() && !info[{{arg.jsArg}}].IsString() && !info[{{arg.jsArg}}].IsObject())) {
    Napi::Error::New(info.Env(), "Array, String Object, or string {{arg.name}} is required.").ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }
        {%else%}
  if (info.Length() == {{arg.jsArg}} || !info[{{arg.jsArg}}].Is{{arg.cppClassName|cppToV8}}()) {
    Napi::Error::New(info.Env(), "{{arg.jsClassName}} {{arg.name}} is required.").ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }
        {%endif%}
      {%endif%}
    {%endif%}
  {%endif%}
{%endeach%}

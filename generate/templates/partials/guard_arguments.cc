
{%each args|argsInfo as arg%}
  {%if arg.isJsArg%}
    {%if not arg.isOptional%}
      {%if not arg.payloadFor %}
        {%if arg | isOid %}
  if (info.Length() == {{arg.jsArg}}
    || (!info[{{arg.jsArg}}]->IsObject() && !info[{{arg.jsArg}}]->IsString())) {
    return Nan::ThrowError("{{arg.jsClassName}} {{arg.name}} is required.");
  }
        {%elsif arg.isCallbackFunction %}
  if (info.Length() == {{arg.jsArg}} || !info[{{arg.jsArg}}]->IsFunction()) {
    return Nan::ThrowError("{{arg.jsClassName}} {{arg.name}} is required.");
  }
        {%elsif arg.cppClassName == "GitStrarray" %}
  if (info.Length() == {{arg.jsArg}} || !(Nan::To<bool>(info[{{arg.jsArg}}]).FromJust())) {
    return Nan::ThrowError("Array, String Object, or string {{arg.name}} is required.");
  }
        {%else%}
  if (info.Length() == {{arg.jsArg}} || !info[{{arg.jsArg}}]->Is{{arg.cppClassName|cppToV8}}()) {
    return Nan::ThrowError("{{arg.jsClassName}} {{arg.name}} is required.");
  }

        {%endif%}
      {%endif%}
    {%endif%}
  {%endif%}
{%endeach%}

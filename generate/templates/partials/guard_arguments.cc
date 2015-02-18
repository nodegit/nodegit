
{%each args|argsInfo as arg%}
  {%if arg.isJsArg%}
    {%if not arg.isOptional%}
      {%if not arg.payloadFor %}
        {%if arg | isOid %}
  if (args.Length() == {{arg.jsArg}}
    || (!args[{{arg.jsArg}}]->IsObject() && !args[{{arg.jsArg}}]->IsString())) {
    return NanThrowError("{{arg.jsClassName}} {{arg.name}} is required.");
  }
        {%elsif arg.isCallbackFunction %}
  if (args.Length() == {{arg.jsArg}} || !args[{{arg.jsArg}}]->IsFunction()) {
    return NanThrowError("{{arg.jsClassName}} {{arg.name}} is required.");
  }
        {%elsif arg.cppClassName == "GitStrarray" %}
  if (args.Length() == {{arg.jsArg}} || !args[{{arg.jsArg}}]->BooleanValue()) {
    return NanThrowError("Array, String Object, or string {{arg.name}} is required.");
  }
        {%else%}
  if (args.Length() == {{arg.jsArg}} || !args[{{arg.jsArg}}]->Is{{arg.cppClassName|cppToV8}}()) {
    return NanThrowError("{{arg.jsClassName}} {{arg.name}} is required.");
  }

        {%endif%}
      {%endif%}
    {%endif%}
  {%endif%}
{%endeach%}

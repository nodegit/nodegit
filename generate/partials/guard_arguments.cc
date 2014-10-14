
{%each args|argsInfo as arg%}
  {%if arg.isJsArg%}
    {%if not arg.isOptional%}
  if (args.Length() == {{arg.jsArg}} || !args[{{arg.jsArg}}]->Is{{arg.cppClassName|cppToV8}}()) {
    return NanThrowError("{{arg.jsClassName}} {{arg.name}} is required.");
  }

    {%endif%}
  {%endif%}
{%endeach%}

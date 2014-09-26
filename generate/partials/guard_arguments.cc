{%each args|argsInfo as arg%}
  {%if arg.isJsArg%}
    {%if not arg.isOptional%}
    if (args.Length() == {{arg.jsArg}} || !args[{{jsArg}}]->Is{{arg.v8ValueClassName}}()) {
      return NanThrowError("{{arg.jsClassName}} {{arg.name}} is required.");
    }
    {%endif%}
  {%endif%}
{%endeach%}

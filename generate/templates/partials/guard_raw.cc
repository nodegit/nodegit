{%each args|argsInfo as arg %}
  {%if not arg.isReturn %}
    {%if arg.isSelf %}
  if (Nan::ObjectWrap::Unwrap<{{ arg.cppClassName }}>(info.This())->GetValue() == NULL) {
    return Nan::ThrowError("This {{ arg.jsClassName }} has been freed and can no longer be used.");
  }
    {%endif%}
  {%endif%}
{%endeach%}

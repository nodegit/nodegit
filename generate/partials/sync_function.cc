/**
{%partial doc .%}
 */
NAN_METHOD({{ cppClassName }}::{{ cppFunctionName }}) {
  NanScope();
  {%partial guardArguments .%}

  {%each arg|argsInfo as arg %}
    {%if arg.isReturn %}
      {%if arg.shouldAlloc %}
  {{ arg.cType }}{{ arg.name }} = ({{ arg.cType }})malloc(sizeof({{ arg.cType|unPointer }}));
      {%else%}
  {{ arg.cType|unPointer }} {{ arg.name }} = {{ arg.cType|unPointer|defaultValue }};
      {%endif%}
    {%endif%}
  {%endeach%}

  {%each args|argsInfo as arg %}
    {%if not arg.isSelf %}
      {%if not arg.isReturn %}
        {%partial convertFromV8 arg %}
      {%endif%}
    {%endif%}
  {%endeach%}

{%if .|hasReturns %}
  {{ return.cType }} result = {%endif%}{{ cFunctionName }}(
  {%each args|argsInfo as arg %}
    {%if arg.isReturn %}
      {%if not arg.shouldAlloc %}
    &
      {%endif%}
    {%endif%}
    {%if arg.isSelf %}
ObjectWrap::Unwrap<{{ arg.cppClassName }}>(args.This())->GetValue()
    {%elsif arg.isReturn %}
{{ arg.name }}
    {%else%}
from_{{ arg.name }}
    {%endif%}
    {%if not arg.lastArg %},{%endif%}
  {%endeach%}
  );

{%each args|argsInfo as arg %}
  {%if arg.isCppClassStringOrArray %}
    {%if arg.freeFunctionName %}
  {{ arg.freeFunctionName }}(from_{{ arg.name }});
    {%else%}
  free((void *)from_{{ arg.name }});
    {%endif%}
  {%endif%}
{%endeach%}

{%if return.isErrorCode %}
  if (result != GIT_OK) {
  {%each args|argsInfo as arg %}
    {%if arg.shouldAlloc %}
    free({{ arg.name }});
    {%endif%}
  {%endeach%}

    if (giterr_last()) {
      return NanThrowError(giterr_last()->message);
    } else {
      return NanThrowError("Unknown Error");
    }
  }
{%endif%}

{%if not .|returnsCount %}
  NanReturnUndefined();
{%else%}
  Handle<Value> to;
  {%if .|returnsCount > 1 %}
  Handle<Object> toReturn = NanNew<Object>();
  {%endif%}
  {%each .|returnsInfo as _return %}
    {%partial convertToV8 _return %}
    {%if .|returnsCount > 1 %}
  toReturn->Set(NanNew<String>("{{ _return.jsNameOrName }}"), to);
    {%endif%}
  {%endeach%}
  {%if .|returnsCount == 1 %}
  NanReturnValue(to);
  {%else%}
  NanReturnValue(toReturn);
  {%endif%}
{%endif%}
}

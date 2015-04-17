
{%partial doc .%}
NAN_METHOD({{ cppClassName }}::{{ cppFunctionName }}) {
  NanEscapableScope();
  {%partial guardArguments .%}

  {%each .|returnsInfo 'true' as _return %}
    {%if _return.shouldAlloc %}
  {{ _return.cType }}{{ _return.name }} = ({{ _return.cType }})malloc(sizeof({{ _return.cType|unPointer }}));
    {%else%}
  {{ _return.cType|unPointer }} {{ _return.name }} = {{ _return.cType|unPointer|defaultValue }};
    {%endif%}
  {%endeach%}

  {%each args|argsInfo as arg %}
    {%if not arg.isSelf %}
      {%if not arg.isReturn %}
        {%partial convertFromV8 arg %}
        {%if arg.saveArg %}
  Handle<Object> {{ arg.name }}(args[{{ arg.jsArg }}]->ToObject());
  {{ cppClassName }} *thisObj = ObjectWrap::Unwrap<{{ cppClassName }}>(args.This());

  NanDisposePersistent(thisObj->{{ cppFunctionName }}_{{ arg.name }});

  NanAssignPersistent(thisObj->{{ cppFunctionName }}_{{ arg.name }}, {{ arg.name }});
        {%endif%}
      {%endif%}
    {%endif%}
  {%endeach%}

{%each args|argsInfo as arg %}
{%endeach%}

{%-- Inside a free call, if the value is already free'd don't do it again.--%}
{% if cppFunctionName == "Free" %}
if (ObjectWrap::Unwrap<{{ cppClassName }}>(args.This())->GetValue() != NULL) {
{% endif %}

{%if .|hasReturnValue %}
  {{ return.cType }} result = {%endif%}{{ cFunctionName }}(
  {%each args|argsInfo as arg %}
    {%if arg.isReturn %}
      {%if not arg.shouldAlloc %}&{%endif%}
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

{%if .|hasReturnValue |and return.isErrorCode %}
  if (result != GIT_OK) {
  {%each args|argsInfo as arg %}
    {%if arg.shouldAlloc %}
    free({{ arg.name }});
    {%elsif arg | isOid %}
    if (args[{{ arg.jsArg }}]->IsString()) {
      free({{ arg.name }});
    }
    {%endif%}
  {%endeach%}

    if (giterr_last()) {
      return NanThrowError(giterr_last()->message);
    } else {
      return NanThrowError("Unknown Error");
    }
  }
{%endif%}

{% if cppFunctionName == "Free" %}
  ObjectWrap::Unwrap<{{ cppClassName }}>(args.This())->ClearValue();
}
{% endif %}


{%each args|argsInfo as arg %}
  {%if arg | isOid %}
  if (args[{{ arg.jsArg }}]->IsString()) {
    free((void *)from_{{ arg.name }});
  }
  {%endif%}
{%endeach%}

{%if not .|returnsCount %}
  NanReturnUndefined();
{%else%}
  {%if return.cType | isPointer %}
  // null checks on pointers
  if (!result) {
    NodeGitPsueodoNanReturnEscapingValue(NanUndefined());
  }
  {%endif%}

  Handle<v8::Value> to;
  {%if .|returnsCount > 1 %}
  Handle<Object> toReturn = NanNew<Object>();
  {%endif%}
  {%each .|returnsInfo as _return %}
    {%partial convertToV8 _return %}
    {%if .|returnsCount > 1 %}
  toReturn->Set(NanNew<String>("{{ _return.returnNameOrName }}"), to);
    {%endif%}
  {%endeach%}
  {%if .|returnsCount == 1 %}
  NodeGitPsueodoNanReturnEscapingValue(to);
  {%else%}
  NodeGitPsueodoNanReturnEscapingValue(toReturn);
  {%endif%}
{%endif%}
}

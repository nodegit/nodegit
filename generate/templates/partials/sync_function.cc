
{%partial doc .%}
NAN_METHOD({{ cppClassName }}::{{ cppFunctionName }}) {
  Nan::EscapableHandleScope scope;
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
  Local<Object> {{ arg.name }}(info[{{ arg.jsArg }}]->ToObject());
  {{ cppClassName }} *thisObj = Nan::ObjectWrap::Unwrap<{{ cppClassName }}>(info.This());

  thisObj->{{ cppFunctionName }}_{{ arg.name }}.Reset({{ arg.name }});
        {%endif%}
      {%endif%}
    {%endif%}
  {%endeach%}

{%each args|argsInfo as arg %}
{%endeach%}

{%-- Inside a free call, if the value is already free'd don't do it again.--%}
{% if cppFunctionName == "Free" %}
if (Nan::ObjectWrap::Unwrap<{{ cppClassName }}>(info.This())->GetValue() != NULL) {
{% endif %}

  giterr_clear();

  {
    LockMaster lockMaster(/*asyncAction: */false{%each args|argsInfo as arg %}
      {%if arg.cType|isPointer%}{%if not arg.isReturn%}
        ,{%if arg.isSelf %}
    Nan::ObjectWrap::Unwrap<{{ arg.cppClassName }}>(info.This())->GetValue()
        {%else%}
    from_{{ arg.name }}
      {%endif%}
      {%endif%}{%endif%}
    {%endeach%});

  {%if .|hasReturnValue %}
    {{ return.cType }} result = {%endif%}{{ cFunctionName }}(
    {%each args|argsInfo as arg %}
      {%if arg.isReturn %}
        {%if not arg.shouldAlloc %}&{%endif%}
      {%endif%}
      {%if arg.isSelf %}
  Nan::ObjectWrap::Unwrap<{{ arg.cppClassName }}>(info.This())->GetValue()
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
      if (info[{{ arg.jsArg }}]->IsString()) {
        free({{ arg.name }});
      }
      {%endif%}
    {%endeach%}

      if (giterr_last()) {
        return Nan::ThrowError(giterr_last()->message);
      } else {
        return Nan::ThrowError("Unknown Error");
      }
    }
  {%endif%}

  {% if cppFunctionName == "Free" %}
    Nan::ObjectWrap::Unwrap<{{ cppClassName }}>(info.This())->ClearValue();
  }
  {% endif %}


  {%each args|argsInfo as arg %}
    {%if arg | isOid %}
    if (info[{{ arg.jsArg }}]->IsString()) {
      free((void *)from_{{ arg.name }});
    }
    {%endif%}
  {%endeach%}

  {%if not .|returnsCount %}
    return info.GetReturnValue().Set(scope.Escape(Nan::Undefined()));
  {%else%}
    {%if return.cType | isPointer %}
    // null checks on pointers
    if (!result) {
      return info.GetReturnValue().Set(scope.Escape(Nan::Undefined()));
    }
    {%endif%}

    Local<v8::Value> to;
    {%if .|returnsCount > 1 %}
    Local<Object> toReturn = Nan::New<Object>();
    {%endif%}
    {%each .|returnsInfo as _return %}
      {%partial convertToV8 _return %}
      {%if .|returnsCount > 1 %}
    Nan::Set(toReturn, Nan::New("{{ _return.returnNameOrName }}").ToLocalChecked(), to);
      {%endif%}
    {%endeach%}
    {%if .|returnsCount == 1 %}
    return info.GetReturnValue().Set(scope.Escape(to));
    {%else%}
    return info.GetReturnValue().Set(scope.Escape(toReturn));
    {%endif%}
  {%endif%}
  }
}

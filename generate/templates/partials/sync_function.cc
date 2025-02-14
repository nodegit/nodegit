
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
          v8::Local<Object> {{ arg.name }}(Nan::To<v8::Object>(info[{{ arg.jsArg }}]).ToLocalChecked());
          {{ cppClassName }} *thisObj = Nan::ObjectWrap::Unwrap<{{ cppClassName }}>(info.This());

          thisObj->{{ cppFunctionName }}_{{ arg.name }}.Reset({{ arg.name }});
        {%endif%}
      {%endif%}
    {%endif%}
  {%endeach%}

  {%-- Inside a free call, if the value is already free'd don't do it again.--%}
  {%if cppFunctionName == "Free" %}
    if (Nan::ObjectWrap::Unwrap<{{ cppClassName }}>(info.This())->GetValue() != NULL) {
  {%endif%}

  git_error_clear();

  { // lock master scope start
    nodegit::LockMaster lockMaster(
      /*asyncAction: */false
      {%each args|argsInfo as arg %}
        {%if arg.cType|isPointer%}
          {%if not arg.isReturn%}
            ,
            {%if arg.isSelf %}
              Nan::ObjectWrap::Unwrap<{{ arg.cppClassName }}>(info.This())->GetValue()
            {%else%}
              from_{{ arg.name }}
            {%endif%}
          {%endif%}
        {%endif%}
      {%endeach%}
    );

    {%if .|hasReturnType %} {{ return.cType }} result = {%endif%}
    {{ cFunctionName }}(
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

    {%if .|hasReturnType |and return.isErrorCode %}
      if (result != GIT_OK) {
      {%each args|argsInfo as arg %}
        {%if arg | isOid %}
          if (info[{{ arg.jsArg }}]->IsString()) {
            free((void *)from_{{ arg.name }});
          }
        {%elsif arg.shouldAlloc %}
          free({{ arg.name }});
        {%endif%}
      {%endeach%}

        if (git_error_last()) {
          return Nan::ThrowError(git_error_last()->message);
        } else {
          return Nan::ThrowError("Unknown Error");
        }
      } // lock master scope end
    {%endif%}

    {%if cppFunctionName == "Free" %}
        Nan::ObjectWrap::Unwrap<{{ cppClassName }}>(info.This())->ClearValue();
      } // lock master scope end
    {%endif%}

    {%each args|argsInfo as arg %}
      {%if not arg.isSelf %}
        {%if not arg.payloadFor %}
          {%if not arg.isReturn %}
            {%if arg.cppClassName == 'GitStrarray' %}
              {{ arg.freeFunctionName }}((git_strarray*)from_{{ arg.name }});
              free((void*)from_{{ arg.name }});
            {%elsif arg.cppClassName == 'Array' %}
              free((void*)from_{{ arg.name }});
            {%endif%}
          {%endif%}
        {%endif%}
      {%endif%}
    {%endeach%}

    {%each args|argsInfo as arg %}
      {%if arg.shouldAlloc|and arg.isReturn|and arg.cppClassName == 'Array' %}
        {%if arg.cType == 'git_strarray *' %}
          // We need to free the strarray
          {{ arg.freeFunctionName }}(from_{{ arg.name }});
          free((void *)from_{{ arg.name }});
        {%endif%}
      {%endif%}
      {%if arg.shouldAlloc|and arg.isReturn %}
        {%if not arg.selfFreeing %}
          {%if arg.cppClassName == "GitBuf" %}
          {%else%}
            free((void *)from_{{ arg.name }});
          {%endif%}
        {%endif%}
      {%endif%}
    {%endeach%}

    {%each args|argsInfo as arg %}
      {%if arg | isOid %}
        if (info[{{ arg.jsArg }}]->IsString()) {
          free((void *)from_{{ arg.name }});
        }
      {%else%}
        {%if not arg.isReturn %}
          {%if arg.cppClassName == 'String'%}
            free((void *)from_{{ arg.name }});
          {%endif%}
          {%if arg.cppClassName == 'Wrapper'%}
            free((void *)from_{{ arg.name }});
          {%endif%}
        {%endif%}
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

      v8::Local<v8::Value> v8ConversionSlot;
      {%if .|returnsCount > 1 %}
        v8::Local<Object> toReturn = Nan::New<Object>();
      {%endif%}
      {%each .|returnsInfo as _return %}
        {%partial convertToV8 _return %}
        {%if .|returnsCount > 1 %}
          Nan::Set(toReturn, Nan::New("{{ _return.returnNameOrName }}").ToLocalChecked(), v8ConversionSlot);
        {%endif%}
      {%endeach%}
      {%if .|returnsCount == 1 %}
        return info.GetReturnValue().Set(scope.Escape(v8ConversionSlot));
      {%else%}
        return info.GetReturnValue().Set(scope.Escape(toReturn));
      {%endif%}
    {%endif%}
  }
}

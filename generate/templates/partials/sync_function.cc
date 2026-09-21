
{%partial doc .%}
Napi::Value {{ cppClassName }}::{{ cppFunctionName }}(const Napi::CallbackInfo& info) {
  Napi::EscapableHandleScope scope(info.Env());
  Napi::Env env = info.Env();
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
          Napi::Object {{ arg.name }} = info[{{ arg.jsArg }}].As<Napi::Object>();
          {{ cppClassName }} *thisObj = NodeGitWrapper<{{ cppClassName }}Traits>::Unwrap<{{ cppClassName }}>(info.This().As<Napi::Object>());

          thisObj->{{ cppFunctionName }}_{{ arg.name }} = Napi::Persistent({{ arg.name }});
        {%endif%}
      {%endif%}
    {%endif%}
  {%endeach%}

  {%-- Inside a free call, if the value is already free'd don't do it again.--%}
  {%if cppFunctionName == "Free" %}
    if (NodeGitWrapper<{{ cppClassName }}Traits>::Unwrap<{{ cppClassName }}>(info.This().As<Napi::Object>())->GetValue() != NULL) {
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
              NodeGitWrapper<{{ arg.cppClassName }}Traits>::Unwrap<{{ arg.cppClassName }}>(info.This().As<Napi::Object>())->GetValue()
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
          NodeGitWrapper<{{ arg.cppClassName }}Traits>::Unwrap<{{ arg.cppClassName }}>(info.This().As<Napi::Object>())->GetValue()
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
          if (info[{{ arg.jsArg }}].IsString()) {
            free((void *)from_{{ arg.name }});
          }
        {%elsif arg.shouldAlloc %}
          free({{ arg.name }});
        {%endif%}
      {%endeach%}

        if (git_error_last()->klass != GIT_ERROR_NONE) {
          Napi::Error::New(info.Env(), git_error_last()->message).ThrowAsJavaScriptException();
          return info.Env().Undefined();
        } else {
          Napi::Error::New(info.Env(), "Unknown Error").ThrowAsJavaScriptException();
          return info.Env().Undefined();
        }
      } // lock master scope end
    {%endif%}

    {%if cppFunctionName == "Free" %}
        NodeGitWrapper<{{ cppClassName }}Traits>::Unwrap<{{ cppClassName }}>(info.This().As<Napi::Object>())->ClearValue();
      } // lock master scope end
    {%endif%}
    {%if cFunctionName == "git_branch_delete" %}
      if (result == GIT_OK) {
        NodeGitWrapper<GitRefsTraits>::Unwrap<GitRefs>(info[0].As<Napi::Object>())->ClearValue();
      }
    {%endif%}


    {%each args|argsInfo as arg %}
      {%if arg | isOid %}
      if (info[{{ arg.jsArg }}].IsString()) {
        free((void *)from_{{ arg.name }});
      }
      {%endif%}
    {%endeach%}

    {%if not .|returnsCount %}
      return scope.Escape(info.Env().Undefined());
    {%else%}
      {%if return.cType | isPointer %}
        // null checks on pointers
        if (!result) {
          return scope.Escape(info.Env().Undefined());
        }
      {%endif%}

      Napi::Value v8ConversionSlot;
      {%if .|returnsCount > 1 %}
        Napi::Object toReturn = Napi::Object::New(info.Env());
      {%endif%}
      {%each .|returnsInfo as _return %}
        {%partial convertToV8 _return %}
        {%if .|returnsCount > 1 %}
          toReturn.Set("{{ _return.returnNameOrName }}", v8ConversionSlot);
        {%endif%}
      {%endeach%}
      {%if .|returnsCount == 1 %}
        return scope.Escape(v8ConversionSlot);
      {%else%}
        return scope.Escape(toReturn);
      {%endif%}
    {%endif%}
  }
}

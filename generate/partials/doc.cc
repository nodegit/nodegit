<%
  for (var i = 0; i < functionInfo.args.length; i++) {
    var arg = functionInfo.args[i];
    if (arg.isReturn || arg.isSelf) continue;
-%>
<% } -%>

<% for (var r = 0; r < returns.length; r++) { -%>
<% if (functionInfo.isAsync) { -%>
<% } else { -%>
<% } -%>
<% } -%>

{%each functionInfo.args%}
  {%if not arg.isReturn and not arg.isSelf%}
  * @param {{{ jsClassName }}} {{ name }}
  {%endif%}

{%endeach%}

{%each returns%}
  {%if functionInfo.isAsync%}
  * @param {{{ jsClassName }}} callback
  {%else%}
  * @return {{{ jsClassName }}} {%if name%}{{ name }}{%else%}{{'result'}}{%endeach%}
  {%endif%}
{%endeach%}

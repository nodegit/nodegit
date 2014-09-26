{%each functionInfo.args as arg %}
  {%if not arg.isReturn %}
    {%if not arg.isSelf %}
  * @param {{ arg.jsClassName }} {{ arg.name }}
    {%endif%}
  {%endif%}
{%endeach%}

{%each functionInfo|returns as returnInfo %}
  {%if functionInfo.isAsync %}
  * @param {{ returnInfo.jsClassName }} callback
  {%else%}
  * @return {{ returnInfo.jsClassName }} {%if returnInfo.name %}{{ returnInfo.name }}{%else%}result{%endif%}
  {%endif%}
{%endeach%}

/**
{%each args as arg %}
  {%if not arg.isReturn %}
    {%if not arg.isSelf %}
 * @param {{ arg.jsClassName }} {{ arg.name }}
    {%endif%}
  {%endif%}
{%endeach%}{%each .|returnsInfo as returnInfo %}
  {%if isAsync %}
 * @param {{ returnInfo.jsOrCppClassName }} callback
  {%else%}
 * @return {{ returnInfo.jsOrCppClassName }} {%if returnInfo.name %}{{ returnInfo.name }}{%else%}result{%endif%}
  {%endif%}
{%endeach%}
 */

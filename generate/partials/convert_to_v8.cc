{%if cppClassName == 'String' %}
  {%if size %}
to = NanNew<String>({{ parsedName }}, {{ size }});
  {%elsif cType == 'char **' %}
to = NanNew<String>(*{{ parsedName }});
  {%else%}}
to = NanNew<String>({{ parsedName }});
  {%endif%}

  {%if freeFunctionName %}
    {{ freeFunctionName }}({{ parsedName }});
  {%endif%}
{%elsif cppClassName|isV8Value %}
  {%if isCppClassIntType %}
to = NanNew<{{ cppClassName }}>(({{ parsedClassName }}){{ parsedName }});
  {%else%}
to = NanNew<{{ cppClassName }}>({{ parsedName }});
  {%endif%}
{%elsif cppClassName == 'External' %}
to = NanNew<External>((void *){{ parsedName }});
{%elsif cppClassName == 'Array' %}
{%--  // FIXME this is not general purpose enough. --%}
Local<Array> tmpArray = NanNew<Array>({{ parsedName }}->{{ size }});
for (unsigned int i = 0; i < {{ parsedName }}->{{ size }}; i++) {
  tmpArray->Set(NanNew<Number>(i), NanNew<String>({{ parsedName }}->{{ key }}[i]));
}
to = tmpArray;
{%else%}
  {%if copy %}
if ({{ parsedName }} != NULL) {
  {{ parsedName }} = ({{ cType|replace '**' '*' }} {%if cType|isPointer %}*{%endif%}){{ copy }}({{ parsedName }});
}
  {%endif%}
if ({{ parsedName }} != NULL) {
  to = {{ cppClassName }}::New((void *){{ parsedName }});
} else {
  to = NanNull();
}
{%endif%}

// start convert_to_v8 block
{%if cppClassName == 'String' %}
  {%if size %}
to = NanNew<String>({{= parsedName =}}, {{ size }});
  {%elsif cType == 'char **' %}
to = NanNew<String>(*{{= parsedName =}});
  {%else%}
to = NanNew<String>({{= parsedName =}});
  {%endif%}

  {%if freeFunctionName %}
{{ freeFunctionName }}({{= parsedName =}});
  {%endif%}
{%elsif cppClassName|isV8Value %}
  {%if isCppClassIntType %}
to = NanNew<{{ cppClassName }}>(({{ parsedClassName }}){{= parsedName =}});
  {%else%}
  to = NanNew<{{ cppClassName }}>({{= parsedName =}});
  {%endif%}
{%elsif cppClassName == 'External' %}
to = NanNew<External>((void *){{= parsedName =}});
{%elsif cppClassName == 'Array' %}
{%--
  // FIXME this is not general purpose enough.
--%}
{%if size%}
Local<Array> tmpArray = NanNew<Array>({{= parsedName =}}->{{ size }});
for (unsigned int i = 0; i < {{= parsedName =}}->{{ size }}; i++) {
  tmpArray->Set(NanNew<Number>(i), NanNew<String>({{= parsedName =}}->{{ key }}[i]));
}
{%else%}
Local<Array> tmpArray = NanNew<Array>({{= parsedName =}});
{%endif%}
to = tmpArray;
{%else%}
  {%if copy %}
  if ({{= parsedName =}} != NULL) {
    {{= parsedName =}} = ({{ cType|replace '**' '*' }} {%if not cType|isPointer %}*{%endif%}){{ copy }}({{= parsedName =}});
  }
  {%endif%}

    if ({{= parsedName =}} != NULL) {
      // {{= cppClassName }} {{= parsedName }}
      {%if cppClassName == 'Wrapper' %}
      to = {{ cppClassName }}::New((void *){{= parsedName =}});
      {%else%}
      to = {{ cppClassName }}::New((void *){{= parsedName =}}, false);
      {%endif%}
    } else {
      to = NanNull();
    }

{%endif%}
// end convert_to_v8 block

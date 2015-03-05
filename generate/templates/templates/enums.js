var NodeGit = require("../");
NodeGit.Enums = {};

{% each . as enumerable %}
  {% if not enumerable.ignore %}
    {% if enumerable.type == "enum" %}
      NodeGit.{{ enumerable.owner }}.{{ enumerable.JsName }} = {
      {% each enumerable.values as value %}
        {{ value.JsName }}: {{ value.value }},
      {% endeach %}
      };
    {% endif %}
  {% endif %}
{% endeach %}

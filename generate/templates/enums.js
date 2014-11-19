var NodeGit = require("../");
NodeGit.Enums = {};

/* jshint ignore:start */
{% each %}
var {{ owner }} = NodeGit.{{ owner }};

  {% each enums as enumerable %}
    {% if enumerable.type == "enum" %}
{{ owner }}.{{ enumerable.JsName }} = {
      {% each enumerable.values as value %}
  {{ value.JsName }}: {{ value.value }},
      {% endeach %}
};
    {% endif %}
  {% endeach %}

{% endeach %}
/* jshint ignore:end */

var NodeGit = require("../");
NodeGit.Enums = {};

/* jshint ignore:start */
{% each . as enumerable %}
{% if enumerable.type == "enum" %}
NodeGit.{{ enumerable.owner }}.{{ enumerable.JsName }} = {
{% each enumerable.values as value %}
  {{ value.JsName }}: {{ value.value }},
{% endeach %}
};

{% endif %}
{% endeach %}

/* jshint ignore:end */

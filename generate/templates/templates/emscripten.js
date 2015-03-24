// Load Emscripten build.
require('../../vendor/libgit2.min');

{% each . as idef %}
  {% if idef.type != "enum" %}
    function {{ idef.jsClassName }}() {

    }

    {% each idef.functions as fn %}
      {% if fn.isPrototypeMethod %}
        {{ idef.jsClassName }}.prototype.{{fn.jsFunctionName}} = function() {
          var retVal = _{{ fn.cFunctionName }}();
            {%--
            <% descriptor.args.forEach(function(arg) { %>
            "<%- (arg.jsClassName || "").toLowerCase() %>",
            <% }); %>
          ], [
            <% if (descriptor.args && descriptor.args[0].isReturn) { %>
            <%- descriptor.args[0].name %>,
            <% } %>
            <%- args(descriptor) %>
          ]);
            --%}
        };

      {% endif %}
    {% endeach %}
  {% endif %}
{% endeach %}


{%--
  {% if fn.isAsync %}

    {% if fn.isPrototypeMethod %}

      var _{{ idef.jsClassName }}_{{ fn.jsFunctionName}}
        = _{{ idef.jsClassName }}.prototype.{{ fn.jsFunctionName }};
      _{{ idef.jsClassName }}.prototype.{{ fn.jsFunctionName }}
        = promisify(_{{ idef.jsClassName }}_{{ fn.jsFunctionName}});

    {% else %}

      var _{{ idef.jsClassName }}_{{ fn.jsFunctionName}}
        = _{{ idef.jsClassName }}.{{ fn.jsFunctionName }};
      _{{ idef.jsClassName }}.{{ fn.jsFunctionName }}
        = promisify(_{{ idef.jsClassName }}_{{ fn.jsFunctionName}});

    {% endif %}
--%}

var Promise = require("nodegit-promise");
var promisify = require("promisify-node");
var rawApi;

// Attempt to load the production release first, if it fails fall back to the
// debug release.
try {
  rawApi = require("../build/Release/nodegit");
}
catch (e) {
  /* istanbul ignore next */
  rawApi = require("../build/Debug/nodegit");
}

// Native methods do not return an identifiable function, so we
// have to override them here
/* jshint ignore:start */
{% each . as idef %}
  {% if idef.type != "enum" %}

    var _{{ idef.jsClassName }}
      = rawApi.{{idef.jsClassName}};

    {% each idef.functions as fn %}
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

      {% endif %}
    {% endeach %}

  {% endif %}
{% endeach %}
/* jshint ignore:end */

// Set the exports prototype to the raw API.
exports.__proto__ = rawApi;

// Import extensions
require("./attr");
require("./blob");
require("./clone");
require("./checkout");
require("./commit");
require("./diff");
require("./index");
require("./merge");
require("./object");
require("./odb");
require("./odb_object");
require("./oid");
require("./patch");
require("./reference");
require("./remote");
require("./revwalk");
require("./repository");
require("./revwalk");
require("./signature");
require("./status");
require("./tree");
require("./tree_entry");

//must go last!
require("./enums");

// Wrap asynchronous methods to return promises.
promisify(exports);

// Set version.
exports.version = require("../package").version;

// Expose Promise implementation.
exports.Promise = Promise;

// Initialize libgit2.
exports.Libgit2.init();

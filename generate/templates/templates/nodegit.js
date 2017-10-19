/* eslint no-unused-vars: "off" */

var promisify = require("promisify-node");
var rawApi;

// Attempt to load the production release first, if it fails fall back to the
// debug release.
try {
  rawApi = require("../build/Release/nodegit.node");
}
catch (ex) {
  /* istanbul ignore next */
  if (ex.code !== "MODULE_NOT_FOUND") {
    throw ex;
  }

  rawApi = require("../build/Debug/nodegit.node");
}

// Native methods do not return an identifiable function, so we
// have to override them here
{% each . as idef %}
  {% if idef.type != "enum" %}

    {% if idef.functions.length > 0 %}
      var _{{ idef.jsClassName }}
        = rawApi.{{ idef.jsClassName }};
    {% endif %}

    {% each idef.functions as fn %}
      {% if fn.isAsync %}

        {% if fn.isPrototypeMethod %}

          var _{{ idef.jsClassName }}_{{ fn.jsFunctionName}}
            = _{{ idef.jsClassName }}.prototype.{{ fn.jsFunctionName }};
          _{{ idef.jsClassName }}.prototype.{{ fn.jsFunctionName }}
            = promisify(
              _{{ idef.jsClassName }}_{{ fn.jsFunctionName}}
            );

        {% else %}

          var _{{ idef.jsClassName }}_{{ fn.jsFunctionName}}
            = _{{ idef.jsClassName }}.{{ fn.jsFunctionName }};
          _{{ idef.jsClassName }}.{{ fn.jsFunctionName }}
            = promisify(
              _{{ idef.jsClassName }}_{{ fn.jsFunctionName}}
            );

        {% endif %}

      {% endif %}
    {% endeach %}

  {% endif %}
{% endeach %}

var _ConvenientPatch = rawApi.ConvenientPatch;
var _ConvenientPatch_hunks = _ConvenientPatch.prototype.hunks;
_ConvenientPatch.prototype.hunks = promisify(_ConvenientPatch_hunks);

var _ConvenientHunk = rawApi.ConvenientHunk;
var _ConvenientHunk_lines = _ConvenientHunk.prototype.lines;
_ConvenientHunk.prototype.lines = promisify(_ConvenientHunk_lines);

var _FilterRegistry = rawApi.FilterRegistry;
var _FilterRegistry_register = _FilterRegistry.register;
_FilterRegistry.register = promisify(_FilterRegistry_register);

var _FilterRegistry_unregister = _FilterRegistry.unregister;
_FilterRegistry.unregister = promisify(_FilterRegistry_unregister);


// Set the exports prototype to the raw API.
Object.setPrototypeOf(exports, rawApi)

var importExtension = function(name) {
  try {
    require("./" + name);
  }
  catch (unhandledException) {
    if (unhandledException.code != "MODULE_NOT_FOUND") {
      throw unhandledException;
    }
  }
};

// Load up utils
rawApi.Utils = {};
require("./utils/lookup_wrapper");
require("./utils/normalize_options");
require("./utils/shallow_clone");
require("./utils/normalize_fetch_options");

// Load up extra types;
require("./status_file");
require("./enums.js");

// Import extensions
// [Manual] extensions
importExtension("filter_registry");
{% each %}
  {% if type != "enum" %}
    importExtension("{{ filename }}");
  {% endif %}
{% endeach %}
{% each . as idef %}
  {% if idef.type != "enum" %}
    {% each idef.functions as fn %}
      {% if fn.useAsOnRootProto %}

        // Inherit directly from the original {{idef.jsClassName}} object.
        Object.setPrototypeOf(
          _{{ idef.jsClassName }}.{{ fn.jsFunctionName }},
          _{{ idef.jsClassName }}
        );

        // Ensure we're using the correct prototype.
        _{{ idef.jsClassName }}.{{ fn.jsFunctionName }}.prototype =
          _{{ idef.jsClassName }}.prototype;

        // Assign the function as the root
        rawApi.{{ idef.jsClassName }} =
          _{{ idef.jsClassName }}.{{ fn.jsFunctionName }};

      {% endif %}
    {% endeach %}
  {% endif %}
{% endeach %}

// Wrap asynchronous methods to return promises.
promisify(exports);

// Set version.
exports.version = require("../package").version;

// Expose Promise implementation.
exports.Promise = Promise;

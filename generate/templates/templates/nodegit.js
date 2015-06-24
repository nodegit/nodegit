var Promise = require("bluebird");
var rawApi;

function promisify(source) {
  var destination = {};
  if (typeof source === "function") {
    return thenify(source);
  } else {
    Object.keys(source).forEach(function (name) {
      if (typeof source[name] === "function") {
        destination[name] = thenify(source[name]);
      }
    });
  }
}

function thenify ($$__fn__$$) {
  return eval(createWrapper($$__fn__$$.name));
}

function createWrapper (name) {
  name = name ? ~[
    // js reserved words
    // "abstract", "arguments", "boolean", "break", "byte", "case", "catch",
    // "char", "class", "const", "continue", "debugger", "while", "with",
    // "yield", "do", "double", "else", "enum", "eval", "export", "extends",
    // "false", "final", "finally", "float", "for", "function", "goto", "if",
    // "implements", "import", "in", "instanceof", "int", "interface", "let",
    // "long", "native", "new", "null", "package", "private", "protected",
    // "public", "return", "short", "static", "super", "switch", "synchronized",
    // "this", "throw", "throws", "transient", "true", "try", "typeof", "var",
    // "void", "volatile",

    "default", "delete"
  ].indexOf(name) ? "$$" + name : name : "";

  /* jshint ignore:start */
  return "(function " + name + "() {\n"
    + "var self = this\n"
    + "var len = arguments.length\n"
    + "var args = new Array(len + 1)\n"
    + "for (var i = 0; i < len; ++i) args[i] = arguments[i]\n"
    + "var lastIndex = i\n"
    + "return new Promise(function (resolve, reject) {\n"
      + "args[lastIndex] = createCallback(resolve, reject)\n"
      + "$$__fn__$$.apply(self, args)\n"
    + "})\n"
  + "})"
  /* jshint ignore:end */
}

/* jshint ignore:start */
function createCallback(resolve, reject) {
  return function(err, value) {
    if (err) return reject(err)
    var length = arguments.length
    if (length <= 2) return resolve(value)
    var values = new Array(length - 1)
    for (var i = 1; i < length; ++i) values[i - 1] = arguments[i]
    resolve(values)
  }
}
/* jshint ignore:end */




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
/* jshint ignore:start */
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

// Load up extra types;
require("./convenient_line");
require("./convenient_hunk");
require("./convenient_patch");
require("./status_file");
require("./enums.js");

// Import extensions
{% each %}
  {% if type != "enum" %}
    importExtension("{{ filename }}");
  {% endif %}
{% endeach %}
/* jshint ignore:start */
{% each . as idef %}
  {% if idef.type != "enum" %}
    {% each idef.functions as fn %}
      {% if fn.useAsOnRootProto %}

        // Inherit directly from the original {{idef.jsClassName}} object.
        _{{ idef.jsClassName }}.{{ fn.jsFunctionName }}.__proto__ =
          _{{ idef.jsClassName }};

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
/* jshint ignore:end */

// Wrap asynchronous methods to return promises.
promisify(exports);

// Set version.
exports.version = require("../package").version;

// Expose Promise implementation.
exports.Promise = Promise;

// Initialize libgit2.
exports.Libgit2.init();

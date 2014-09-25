const path = require("path");
const combyne = require("combyne");
const file = require("./util/file");
const idefs = require("./idefs");

// Customize the delimiters so as to not process `{{{` or `}}}`.
combyne.settings.delimiters = {
  START_RAW: "{{=",
  END_RAW: "=}}"
};

var partials = {
  asyncFunction: file.read("partials/async_function.cc"),
  convertFromV8: file.read("partials/convert_from_v8.cc"),
  convertToV8: file.read("partials/convert_to_v8.cc"),
  doc: file.read("partials/doc.cc"),
  fields: file.read("partials/fields.cc"),
  guardArguments: file.read("partials/guard_arguments.cc"),
  syncFunction: file.read("partials/sync_function.cc")
};

var templates = {
  class: file.read("templates/class.cc"),
  header: file.read("templates/header.h"),
  binding: file.read("templates/binding.gyp"),
  nodegit: file.read("templates/nodegit.cc")
};

var filters = {
  upper: require("./filters/upper"),
  replace: require("./filters/replace"),
  or: require("./filters/or"),
  returns: require("./filters/returns"),
  argsInfo: require("./filters/args_info"),
  jsArgsCount: require("./filters/js_args_count"),
  isV8Value: require("./filters/is_v8_value"),
  hasReturnType: require("./filters/has_return_type"),
  convertReturns: require("./filters/convert_returns")
};

// Convert Buffers to Combyne templates.
Object.keys(templates).forEach(function(template) {
  templates[template] = combyne(templates[template]);

  // Attach all filters to all templates.
  Object.keys(filters).forEach(function(filter) {
    templates[template].registerFilter(filter, filters[filter]);
  });
});

// Attach all partials to select templates.
Object.keys(partials).forEach(function(partial) {
 templates.class.registerPartial(partial, combyne(partials[partial]));
});


// Determine which definitions to actually include in the source code.
var enabled = idefs.filter(function(idef) {
  // Additionally provide a friendly name to the actual filename.
  idef.name = path.basename(idef.filename, ".h");
  return !idef.ignore;
});

// TODO Wipe out all existing source files.

// Write out single purpose templates.
file.write("../binding.gyp", templates.binding.render(enabled));
file.write("../src/nodegit.cc", templates.nodegit.render(enabled));

// Write out all the classes.
enabled.forEach(function(idef) {
  file.write("../src/" + idef.name + ".cc", templates.class.render(idef));
  file.write("../include/" + idef.name + ".h", templates.header.render(idef));
});

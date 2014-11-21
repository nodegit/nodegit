const path = require("path");
const combyne = require("combyne");
const file = require("./util/file");
const idefs = require("./idefs");
const promisify = require("promisify-node");
const fse = promisify(require("fs-extra"));

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
  syncFunction: file.read("partials/sync_function.cc"),
  fieldAccessors: file.read("partials/field_accessors.cc")
};

var templates = {
  class_content: file.read("templates/class_content.cc"),
  struct_content: file.read("templates/struct_content.cc"),
  class_header: file.read("templates/class_header.h"),
  struct_header: file.read("templates/struct_header.h"),
  binding: file.read("templates/binding.gyp"),
  nodegit: file.read("templates/nodegit.cc"),
  enums: file.read("templates/enums.js")
};

var filters = {
  upper: require("./filters/upper"),
  replace: require("./filters/replace"),
  titleCase: require("./filters/title_case"),
  or: require("./filters/or"),
  and: require("./filters/and"),
  defaultValue: require("./filters/default_value"),
  argsInfo: require("./filters/args_info"),
  cppToV8: require("./filters/cpp_to_v8"),
  jsArgsCount: require("./filters/js_args_count"),
  isV8Value: require("./filters/is_v8_value"),
  isPointer: require("./filters/is_pointer"),
  isDoublePointer: require("./filters/is_double_pointer"),
  unPointer: require("./filters/un_pointer"),
  payloadFor: require("./filters/payload_for"),
  hasReturnType: require("./filters/has_return_type"),
  hasReturns: require("./filters/has_returns"),
  returnsCount: require("./filters/returns_count"),
  returnsInfo: require("./filters/returns_info"),
  fieldsInfo: require("./filters/fields_info")
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
  templates.class_content.registerPartial(partial, combyne(partials[partial]));
  templates.struct_content.registerPartial(partial, combyne(partials[partial]));
});


// Determine which definitions to actually include in the source code.
// This might not be needed anymore but to be frank I'm not totally positive
var enabled = idefs.filter(function(idef) {
  return !idef.ignore;
});

fse.remove(path.resolve(__dirname, "../src")).then(function() {
  return fse.remove(path.resolve(__dirname, "../include"));
}).then(function() {
  return fse.copy(path.resolve(__dirname, "./manual/"), path.resolve(__dirname, "../"));
}).then(function() {
  // Write out single purpose templates.
  file.write("../binding.gyp", templates.binding.render(enabled));
  file.write("../src/nodegit.cc", templates.nodegit.render(enabled));


  // Write out all the classes.
  enabled.forEach(function(idef) {
    try {
      if (idef.type == "struct") {
        file.write("../src/" + idef.filename + ".cc", templates.struct_content.render(idef));
        file.write("../include/" + idef.filename + ".h", templates.struct_header.render(idef));
      }
      else if (idef.type == "class") {
        file.write("../src/" + idef.filename + ".cc", templates.class_content.render(idef));
        file.write("../include/" + idef.filename + ".h", templates.class_header.render(idef));
      }
    }
    catch (e) {
      console.log(e);
    }
  });

  file.write("../lib/enums.js", templates.enums.render(enabled));
});

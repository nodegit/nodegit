const path = require("path");
const promisify = require("promisify-node");
const fse = promisify(require("fs-extra"));
const exec = promisify(function(command, opts, callback) {
  return require("child_process").exec(command, opts, callback);
});

const utils = require("./utils");

module.exports = function generateNativeCode() {
  const combyne = require("combyne");
  const js_beautify = require("js-beautify").js_beautify;
  const beautify = function (input) {
    return js_beautify(input, {
      "brace_style": "end-expand",
      "max_preserve_newlines": 2,
      "preserve_newlines": true,
      "indent_size": 2,
      "indent_char": " "
    });
  };

  // Customize the delimiters so as to not process `{{{` or `}}}`.
  combyne.settings.delimiters = {
    START_RAW: "{{=",
    END_RAW: "=}}"
  };

  var partials = {
    asyncFunction: utils.readFile("templates/partials/async_function.cc"),
    callbackHelpers: utils.readFile("templates/partials/callback_helpers.cc"),
    convertFromV8: utils.readFile("templates/partials/convert_from_v8.cc"),
    convertToV8: utils.readFile("templates/partials/convert_to_v8.cc"),
    doc: utils.readFile("templates/partials/doc.cc"),
    fields: utils.readFile("templates/partials/fields.cc"),
    guardArguments: utils.readFile("templates/partials/guard_arguments.cc"),
    syncFunction: utils.readFile("templates/partials/sync_function.cc"),
    fieldAccessors: utils.readFile("templates/partials/field_accessors.cc")
  };

  var templates = {
    class_content: utils.readFile("templates/templates/class_content.cc"),
    struct_content: utils.readFile("templates/templates/struct_content.cc"),
    class_header: utils.readFile("templates/templates/class_header.h"),
    struct_header: utils.readFile("templates/templates/struct_header.h"),
    binding: utils.readFile("templates/templates/binding.gyp"),
    nodegitCC: utils.readFile("templates/templates/nodegit.cc"),
    nodegitJS: utils.readFile("templates/templates/nodegit.js"),
    enums: utils.readFile("templates/templates/enums.js")
  };

  var filters = {
    and: require("../templates/filters/and"),
    argsInfo: require("../templates/filters/args_info"),
    cppToV8: require("../templates/filters/cpp_to_v8"),
    defaultValue: require("../templates/filters/default_value"),
    fieldsInfo: require("../templates/filters/fields_info"),
    hasReturnType: require("../templates/filters/has_return_type"),
    hasReturnValue: require("../templates/filters/has_return_value"),
    isDoublePointer: require("../templates/filters/is_double_pointer"),
    isFixedLengthString: require("../templates/filters/is_fixed_length_string"),
    isOid: require("../templates/filters/is_oid"),
    isPayload: require("../templates/filters/is_payload"),
    isPointer: require("../templates/filters/is_pointer"),
    isV8Value: require("../templates/filters/is_v8_value"),
    jsArgsCount: require("../templates/filters/js_args_count"),
    or: require("../templates/filters/or"),
    payloadFor: require("../templates/filters/payload_for"),
    replace: require("../templates/filters/replace"),
    returnsCount: require("../templates/filters/returns_count"),
    returnsInfo: require("../templates/filters/returns_info"),
    titleCase: require("../templates/filters/title_case"),
    unPointer: require("../templates/filters/un_pointer"),
    upper: require("../templates/filters/upper")
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
  const idefs = require("../output/idefs");
  var enabled = idefs.filter(function(idef) {
    return !idef.ignore;
  });


  fse.remove(path.resolve(__dirname, "../../src")).then(function() {
    return fse.remove(path.resolve(__dirname, "../../include"));
  }).then(function() {
    return fse.copy(path.resolve(__dirname, "../templates/manual/"), path.resolve(__dirname, "../../"));
  }).then(function() {
    // Write out single purpose templates.
    utils.writeFile("../binding.gyp", beautify(templates.binding.render(enabled)));
    utils.writeFile("../src/nodegit.cc", templates.nodegitCC.render(enabled));
    utils.writeFile("../lib/nodegit.js", beautify(templates.nodegitJS.render(enabled)));
    // Write out all the classes.
    enabled.forEach(function(idef) {
      if (idef.type && idef.type != "enum") {
        utils.writeFile(
          "../src/" + idef.filename + ".cc", templates[idef.type + "_content"].render(idef)
        );
        utils.writeFile(
          "../include/" + idef.filename + ".h", templates[idef.type + "_header"].render(idef)
        );
      }
    });

    utils.writeFile("../lib/enums.js", beautify(templates.enums.render(enabled)));
  }).then(function() {
    return exec("command -v astyle").then(function(astyle) {
      if (astyle) {
        return exec(
          "astyle --options=\".astylerc\" "
          + path.resolve(__dirname, "../../src") + "/*.cc "
          + path.resolve(__dirname, "../../include") + "/*.h"
        ).then(function() {
          return exec(
            "rm "
            + path.resolve(__dirname, "../../src") + "/*.cc.orig "
            + path.resolve(__dirname, "../../include") + "/*.h.orig "
          );
        });
      }
    }, function() {})
  }).catch(console.log);

};

if (require.main === module) {
  module.exports();
}

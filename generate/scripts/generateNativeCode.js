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
    asyncFunction: utils.readFile("combyne/partials/async_function.cc"),
    convertFromV8: utils.readFile("combyne/partials/convert_from_v8.cc"),
    convertToV8: utils.readFile("combyne/partials/convert_to_v8.cc"),
    doc: utils.readFile("combyne/partials/doc.cc"),
    fields: utils.readFile("combyne/partials/fields.cc"),
    guardArguments: utils.readFile("combyne/partials/guard_arguments.cc"),
    syncFunction: utils.readFile("combyne/partials/sync_function.cc"),
    fieldAccessors: utils.readFile("combyne/partials/field_accessors.cc")
  };

  var templates = {
    class_content: utils.readFile("combyne/templates/class_content.cc"),
    struct_content: utils.readFile("combyne/templates/struct_content.cc"),
    class_header: utils.readFile("combyne/templates/class_header.h"),
    struct_header: utils.readFile("combyne/templates/struct_header.h"),
    binding: utils.readFile("combyne/templates/binding.gyp"),
    nodegitCC: utils.readFile("combyne/templates/nodegit.cc"),
    nodegitJS: utils.readFile("combyne/templates/nodegit.js"),
    enums: utils.readFile("combyne/templates/enums.js")
  };

  var filters = {
    upper: require("../combyne/filters/upper"),
    replace: require("../combyne/filters/replace"),
    titleCase: require("../combyne/filters/title_case"),
    or: require("../combyne/filters/or"),
    and: require("../combyne/filters/and"),
    defaultValue: require("../combyne/filters/default_value"),
    argsInfo: require("../combyne/filters/args_info"),
    cppToV8: require("../combyne/filters/cpp_to_v8"),
    jsArgsCount: require("../combyne/filters/js_args_count"),
    isV8Value: require("../combyne/filters/is_v8_value"),
    isPointer: require("../combyne/filters/is_pointer"),
    isDoublePointer: require("../combyne/filters/is_double_pointer"),
    isOid: require("../combyne/filters/is_oid"),
    unPointer: require("../combyne/filters/un_pointer"),
    payloadFor: require("../combyne/filters/payload_for"),
    hasReturnType: require("../combyne/filters/has_return_type"),
    hasReturns: require("../combyne/filters/has_returns"),
    returnsCount: require("../combyne/filters/returns_count"),
    returnsInfo: require("../combyne/filters/returns_info"),
    fieldsInfo: require("../combyne/filters/fields_info")
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
    return fse.copy(path.resolve(__dirname, "../combyne/manual/"), path.resolve(__dirname, "../../"));
  }).then(function() {
    // Write out single purpose templates.
    utils.writeFile("../binding.gyp", beautify(templates.binding.render(enabled)));
    utils.writeFile("../src/nodegit.cc", templates.nodegitCC.render(enabled));
    utils.writeFile("../lib/nodegit.js", beautify(templates.nodegitJS.render(enabled)));
    // Write out all the classes.
    enabled.forEach(function(idef) {
      try {
        if (idef.type && idef.type != "enum") {
          utils.writeFile(
            "../src/" + idef.filename + ".cc", templates[idef.type + "_content"].render(idef)
          );
          utils.writeFile(
            "../include/" + idef.filename + ".h", templates[idef.type + "_header"].render(idef)
          );
        }
      }
      catch (e) {
        if (process.env.BUILD_ONLY) {
          console.error(e);
        }
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
    })
  });

};

if (require.main === module) {
  module.exports();
}

const path = require("path");
const fse = require("fs-extra");
const os = require('os');
const exec = require('../../utils/execPromise');
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
    asyncFunction: utils.readLocalFile("templates/partials/async_function.cc"),
    callbackHelpers: utils.readLocalFile("templates/partials/callback_helpers.cc"),
    convertFromV8: utils.readLocalFile("templates/partials/convert_from_v8.cc"),
    convertToV8: utils.readLocalFile("templates/partials/convert_to_v8.cc"),
    doc: utils.readLocalFile("templates/partials/doc.cc"),
    fields: utils.readLocalFile("templates/partials/fields.cc"),
    guardArguments: utils.readLocalFile("templates/partials/guard_arguments.cc"),
    syncFunction: utils.readLocalFile("templates/partials/sync_function.cc"),
    fieldAccessors: utils.readLocalFile("templates/partials/field_accessors.cc"),
    traits: utils.readLocalFile("templates/partials/traits.h")
  };

  var templates = {
    class_content: utils.readLocalFile("templates/templates/class_content.cc"),
    struct_content: utils.readLocalFile("templates/templates/struct_content.cc"),
    class_header: utils.readLocalFile("templates/templates/class_header.h"),
    struct_header: utils.readLocalFile("templates/templates/struct_header.h"),
    binding: utils.readLocalFile("templates/templates/binding.gyp"),
    nodegitCC: utils.readLocalFile("templates/templates/nodegit.cc"),
    nodegitJS: utils.readLocalFile("templates/templates/nodegit.js"),
    enums: utils.readLocalFile("templates/templates/enums.js")
  };

  var filters = {
    and: require("../templates/filters/and"),
    argsInfo: require("../templates/filters/args_info"),
    arrayTypeToPlainType: require("../templates/filters/array_type_to_plain_type"),
    asElementPointer: require("../templates/filters/as_element_pointer"),
    callbackArgsInfo: require("../templates/filters/callback_args_info"),
    callbackArgsCount: require("../templates/filters/callback_args_count"),
    cppToV8: require("../templates/filters/cpp_to_v8"),
    defaultValue: require("../templates/filters/default_value"),
    fieldsInfo: require("../templates/filters/fields_info"),
    hasReturnType: require("../templates/filters/has_return_type"),
    hasReturnValue: require("../templates/filters/has_return_value"),
    isArrayType: require("../templates/filters/is_array_type"),
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
    subtract: require("../templates/filters/subtract"),
    titleCase: require("../templates/filters/title_case"),
    toBool: require('../templates/filters/to_bool'),
    toSizeOfArray: require("../templates/filters/to_size_of_array"),
    unPointer: require("../templates/filters/un_pointer"),
    setUnsigned: require("../templates/filters/unsigned"),
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
    templates.class_header.registerPartial(partial, combyne(partials[partial]));
    templates.class_content.registerPartial(partial, combyne(partials[partial]));
    templates.struct_header.registerPartial(partial, combyne(partials[partial]));
    templates.struct_content.registerPartial(partial, combyne(partials[partial]));
  });


  // Determine which definitions to actually include in the source code.
  // This might not be needed anymore but to be frank I'm not totally positive
  const idefs = require("../output/idefs");
  var enabled = idefs.filter(function(idef) {
    return !idef.ignore;
  });

  const tempDirPath = path.join(os.tmpdir(), 'nodegit_build');
  const tempSrcDirPath = path.join(tempDirPath, "src");
  const tempIncludeDirPath = path.join(tempDirPath, "include");

  const finalSrcDirPath = path.join(__dirname, '../../src');
  const finalIncludeDirPath = path.join(__dirname, '../../include');

  fse.remove(tempDirPath).then(function() {
    return fse.copy(path.resolve(__dirname, "../templates/manual/include"), tempIncludeDirPath);
  }).then(function() {
    return fse.copy(path.resolve(__dirname, "../templates/manual/src"), tempSrcDirPath);
  }).then(function() {
    // Write out single purpose templates.
    utils.writeLocalFile("../binding.gyp", beautify(templates.binding.render(enabled)), "binding.gyp");
    utils.writeFile(path.join(tempSrcDirPath, "nodegit.cc"), templates.nodegitCC.render(enabled), "nodegit.cc");
    utils.writeLocalFile("../lib/nodegit.js", beautify(templates.nodegitJS.render(enabled)), "nodegit.js");
    // Write out all the classes.
    enabled.forEach(function(idef) {
      if (idef.type && idef.type != "enum") {
        utils.writeFile(
          path.join(tempSrcDirPath, idef.filename + ".cc"),
          templates[idef.type + "_content"].render(idef),
          idef.type + "_content.cc"
        );

        utils.writeFile(
          path.join(tempIncludeDirPath, idef.filename + ".h"),
          templates[idef.type + "_header"].render(idef),
          idef.type + "_header.h"
        );
      }
    });

    utils.writeLocalFile("../lib/enums.js", beautify(templates.enums.render(enabled)), "enums.js");
  }).then(function() {
    return exec("command -v astyle").then(function(astyle) {
      if (astyle) {
        return exec(
          "astyle --options=\".astylerc\" "
          + tempSrcDirPath + "/*.cc "
          + tempIncludeDirPath + "/*.h"
        ).then(function() {
          return exec(
            "rm "
            + tempSrcDirPath + "/*.cc.orig "
            + tempIncludeDirPath + "/*.h.orig "
          );
        });
      }
    }, function() {})
  }).then(function() {
    return Promise.all([
      utils.syncDirs(tempSrcDirPath, finalSrcDirPath),
      utils.syncDirs(tempIncludeDirPath, finalIncludeDirPath),
    ]);
  }).then(function() {
    return fse.remove(tempDirPath);
  }).catch(console.log);

};

if (require.main === module) {
  module.exports();
}

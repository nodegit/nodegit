const path = require("path");
const combyne = require("combyne");
const file = require("./util/file");
const idefs = require("./idefs");
var cppToV8 = require("./filters/cpp_to_v8");

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
  and: require("./filters/and"),
  isV8Value: require("./filters/is_v8_value"),
  unPointer: require("./filters/un_pointer"),
  defaultValue: require("./filters/default_value")
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
  idef.cTypeIsUndefined = typeof idef.cType == 'undefined';
  
  // We need some custom data on each of the functions
  idef.functions.forEach(function(fn) {
    fn.cppClassName = idef.cppClassName;

    // generate argument info
    fn.args = fn.args || [];
    fn.argsInfo = [];

    var cArg,
        jsArg,
        v8Values = [
          "Boolean",
          "Number",
          "String",
          "Integer",
          "Int32",
          "Uint32",
          "Date",
          "Function"
        ];;

    for(cArg = 0, jsArg = 0; cArg < fn.args.length; cArg++) {
      var arg = {};

      arg.__proto__ = fn.args[cArg];

      if (!arg.isReturn && !arg.isSelf && !arg.isPayload) {
        arg.isJsArg = true;
        arg.jsArg = jsArg;

        jsArg++;
      }

      if (cArg >= fn.args.length -1) {
        arg.lastArg = true;
      }
      else {
        arg.lastArg = false;
      }

      arg.cArg = cArg;
      arg.v8ValueClassName = cppToV8(arg.cppClassName);
      arg.isCppClassStringOrArray = ~["String", "Array"].indexOf(arg.cppClassName);
      arg.isPointer = /\s*\*\s*/.test(arg.cType);
      arg.isDoublePointer = /\s*\*\*\s*/.test(arg.cType);
      arg.isV8Value = ~v8Values.indexOf(arg.cppClassName);

      fn.argsInfo.push(arg);
    }

    fn.jsArgsCount = jsArg;

    fn.returns = [];

    // generate returns data
    fn.argsInfo.forEach(function(arg) {
      if (!arg.isReturn) return;

      var _return = {};

      _return.__proto__ = arg;

      _return.parsedName = _return.name;
      _return.jsOrCppClassName = _return.jsClassName || _return.cppClassName;
      _return.isCppClassIntType = ~["Uint32", "Int32"].indexOf(_return.cppClassName);
      _return.parsedClassName = (_return.cppClassName || '').toLowerCase() + "_t";
      _return.jsNameOrName = _return.jsName | _return.name;

      fn.returns.push(_return);
    });

    fn.return = fn.return || {};

    if (!fn.returns.length
        && !fn.return.isErrorCode
        && fn.return.cType != "void") {
      var _return = {};

      _return = fn.return;

      _return.v8ValueClassName = cppToV8(_return.cppClassName);
      _return.isCppClassStringOrArray = ~["String", "Array"].indexOf(_return.cppClassName);
      _return.isPointer = /\s*\*\s*/.test(_return.cType);
      _return.isDoublePointer = /\s*\*\*\s*/.test(_return.cType);
      _return.isV8Value = ~v8Values.indexOf(_return.cppClassName);
      _return.parsedName = 'result';
      _return.jsOrCppClassName = _return.jsClassName || _return.cppClassName;
      _return.isCppClassIntType = ~["Uint32", "Int32"].indexOf(_return.cppClassName);
      _return.parsedClassName = (_return.cppClassName || '').toLowerCase() + "_t";
      _return.jsNameOrName = _return.jsName | _return.name;

      fn.returns.push(fn.return);
    }

    if (fn.returns.length === 1) {
      fn.returns[0].parsedName = "baton->" + fn.returns[0].name;
    }

    fn.hasReturns = fn.returns.length || fn.return.isErrorCode;
    fn.hasReturnType = fn.return.cType != "void" || fn.return.isErrorCode;

    // generate fields data
    fn.fields = fn.fields || [];
    fn.fieldInfos = [];

    fn.fields.forEach(function (field) {
      var fieldInfo = {};
      fieldInfo.__proto__ = field;

      fieldInfo.parsedName = field.name || "result";
      fieldInfo.isCppClassIntType = ~["Uin32", "Int32"].indexOf(field.cppClassName);
      fieldInfo.parsedClassName = (field.cppClassName || '').toLowerCase() + "_t";
      fieldInfo.isV8Value = ~v8Values.indexOf(field.cppClassName);

      fn.fieldInfos.push(fieldInfo);
    });
  });

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

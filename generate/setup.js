const fs = require("fs");
const path = require("path");
const utils = require("./utils");
const _ = require("lodash");

var version = require("../package.json").libgit2.version;
var descriptor = require("./descriptor.json");
var libgit2 = require("./v" + version + ".json");

// TODO: When libgit2's docs include callbacks we should be able to remove this
var callbackDefs = require("./callbacks.json");

// Turn the types array into a hashmap of struct values
var structs = libgit2.types.reduce(function(hashMap, current) {
  var structName = current[0];
  var structDefOverrides = descriptor[structName] || {};
  var structDef = current[1];

  // currently skipping enums
  if (structDef.type === "enum") {
    return hashMap;
  }

  structDef.cType = structName;
  structDef.cppClassName = utils.cTypeToCppName(structName);
  structDef.jsClassName = utils.cTypeToJsName(structName);
  structDef.filename = structName.replace("git_", "");

  if (structDef.type === "enum") {
    structDef.isEnum = true;
  }
  else {
    structDef.isStruct = true;
  }

  structDef.isForwardDeclared = structDef.decl === structName;

  if (structDef.isForwardDeclared) {
    return hashMap;
  }

  structDef.fields = structDef.fields || [];
  structDef.functions = [];
  structDef.dependencies = [];
  structDef.hasConstructor
    = structDef.used
    && structDef.used.needs
    && structDef.used.needs.some(function (fnName) {
      return utils.isConstructorFunction(structDef.cType, fnName);
    });

  if (!structDef.isForwardDeclared) {
    var dependencyLookup = {};

    structDef.fields.forEach(function (field) {
      var normalizedType = utils.normalizeCtype(field.type);

      field.cType = field.type;
      field.cppFunctionName = utils.titleCase(field.name);
      field.jsFunctionName = utils.camelCase(field.name);
      field.cppClassName = utils.cTypeToCppName(field.type);
      field.jsClassName = utils.cTypeToJsName(field.type);

      if (utils.isCallbackFunction(field.name)) {
        field.isCallbackFunction = true;

        if (callbackDefs[field.type]) {
          _.merge(field, callbackDefs[field.type]);
        }
        else {
          console.log("WARNGING: Couldn't find callback definition for "
            + field.type);
        }
      }
      else {
        field.isCallbackFunction = false;

        if (field.name === "payload") {
          field.payloadFor = "*";
        }
        else {
          var cbFieldName;

          structDef.fields.some(function (cbField) {
            if (utils.isPayloadFor(cbField.name, field.name)) {
              cbFieldName = cbField.name;
              return true;
            }
          });

          if (cbFieldName) {
            field.payloadFor = cbFieldName;
          }
        }
      }

      if (field.payloadFor) {
        return;
      }

      if (field.isCallbackFunction) {
        field.args.forEach(function (arg) {
          var argNormalizedType = utils.normalizeCtype(arg.cType);
          var argLibgitType;

          arg.cppClassName = utils.cTypeToCppName(arg.cType);
          arg.jsClassName = utils.cTypeToJsName(arg.cType);
          arg.isLibgitType = libgit2.types.some(function (type) {
            if (type[0] === argNormalizedType) {
              argLibgitType = type[1];
              return true;
            }
          });

          if (arg.isLibgitType) {
            arg.isEnum = argLibgitType.type === "enum";

            if (!arg.isEnum) {
              dependencyLookup[argNormalizedType.replace("git_", "")] = "";
            }
          }
        });
      }

      // We need to find all of the libgit2 types to build the dependency chain
      var libgitType;

      libgit2.types.some(function (type) {
        if (type[0] === normalizedType) {
          libgitType = type[1];
          return true;
        }
      });

      if (libgitType) {
        field.isLibgitType = true;
        field.isEnum = libgitType.type === "enum";

        if (!field.isEnum) {
          dependencyLookup[normalizedType.replace("git_", "")] = "";
        }

        field.hasConstructor
          = libgitType.used
            && libgitType.used.needs
            && libgitType.used.needs.some(function (fnName) {
              return utils.isConstructorFunction(normalizedType, fnName);
            });
      }
    });

    Object.keys(dependencyLookup).forEach(function (dependencyFilename) {
      structDef.dependencies.push("../include/" + dependencyFilename + ".h");
    });

    _.merge(structDef, structDefOverrides);
    hashMap[structName] = structDef;
  }

  return hashMap;
}, {});

var classes = libgit2.groups.reduce(function(hashMap, current) {
  var className = current[0];
  var classDefOverrides = descriptor[className] || {};
  var classDef = {
    isClass: true,
    filename: className,
    cppClassName: utils.cTypeToCppName("git_" + className),
    jsClassName: utils.cTypeToJsName("git_" + className),
    functions: current[1].reduce(function(fnArray, currentFn) {
      var fnDef = libgit2.functions[currentFn];
      fnDef.cFunctionName = currentFn;
      fnArray.push(fnDef);
      return fnArray;
    }, []),
    dependencies: [],
    fields: [],
    ignore: false
  };

  var cType = "git_" + className;
  var hasCtype = libgit2.types.some(function (type) {
    if (type[0] == cType) {
      return true;
    }
  });

  cType = hasCtype ? cType : null;

  classDef.cType = cType;
  var dependencyLookup = {};

  // Decorate functions and calculate dependencies
  classDef.functions.forEach(function(fnDef) {
    var key = fnDef.cFunctionName;

    // if this is the free function for the class, make the ref on the class
    // and then return since we don't want the free functions publicly
    // available
    if (key == classDef.cType + "_free") {
      classDef.freeFunctionName = key;
      fnDef.ignore = true;
      return;
    }

    fnDef.cppFunctionName = utils.cTypeToCppName(key);
    fnDef.jsFunctionName = utils.cTypeToJsName(key);
    fnDef.isAsync = false; // until proven otherwise

    if (fnDef.cppFunctionName == classDef.cppClassName) {
      fnDef.cppFunctionName = fnDef.cppFunctionName.replace("Git", "");
    }

    fnDef.args.forEach(function(arg) {
      var normalizedType = utils.normalizeCtype(arg.type);

      // We need to find all of the libgit2 types to build the dependency chain
      if (structs[normalizedType]) {
        dependencyLookup[normalizedType.replace("git_", "")] = "";
      }

      arg.cType = arg.type;
      arg.cppClassName = utils.cTypeToCppName(normalizedType);
      arg.jsClassName = utils.cTypeToJsName(normalizedType);

      // Mark all of the args that are either returns or are the object
      // itself and determine if this function goes on the prototype
      // or is a constructor method.
      arg.isReturn
        = arg.name === "out"
        || (utils.isDoublePointer(arg.type)
          && normalizedType == classDef.cType);
      arg.isSelf
        = utils.isPointer(arg.type)
        && normalizedType == classDef.cType;

      if (arg.isReturn && classDef.return && classDef.return.type === "int") {
        classDef.return.isErrorCode = true;
        fnDef.isAsync = true;
      }

      if (arg.isReturn && arg.isSelf) {
        arg.isSelf = false;
        fnDef.isConstructorMethod = true;
      }
      else if (arg.isSelf) {
        fnDef.isPrototypeMethod = true;
      }
    });

    if (fnDef.return) {
      fnDef.return.cppClassName = utils.cTypeToCppName(fnDef.return.type);
      fnDef.return.jsClassName = utils.cTypeToJsName(fnDef.return.type);
      fnDef.return.cType = fnDef.return.type;
    }
  });

  Object.keys(dependencyLookup).forEach(function(dependencyFilename) {
    classDef.dependencies.push("../include/" + dependencyFilename + ".h");
  });

  _.merge(classDef, classDefOverrides);
  hashMap[className] = classDef;

  return hashMap;
}, {});

// if there are any collisions between structs and classes, classes will
// overwrite the struct and grab their fields

Object.keys(classes).forEach(function (key) {
  var structKey = key.replace("git_", "");

  if (structs[structKey]) {
    classes[key].fields = structs[structKey].fields;
    structs[structKey].ignore = true;
  }
});

var output = [];

Object.keys(structs).forEach(function (key) {
  output.push(structs[key]);
});

Object.keys(classes).forEach(function (key) {
  output.push(classes[key]);
});

fs.writeFileSync(path.join(__dirname, "idefs.json"),
  JSON.stringify(output, null, 2));

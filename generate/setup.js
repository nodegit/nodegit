const fs = require("fs");
const path = require("path");
const utils = require("./utils");
const _ = require("lodash");

var version = require("../package.json").libgit2.version;
var descriptor = require("./descriptor.json");
var libgit2 = require("./v" + version + ".json");

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

  if (structDef.type === "enum") {
    structDef.isEnum = true;
  }
  else {
    structDef.isStruct = true;
  }

  structDef.isForwardDeclared = structDef.decl === structName;
  structDef.fields = structDef.fields || [];
  structDef.dependencies = [];

  if (!structDef.isForwardDeclared) {
    var dependencyLookup = {};

    structDef.fields.forEach(function (field) {
      var normalizedType = utils.normalizeCtype(field.type);

      field.cType = field.type;
      field.cppFunctionName = utils.titleCase(field.name);
      field.jsFunctionName = utils.camelCase(field.name);
      field.cppClassName = utils.cTypeToCppName(field.type);
      field.jsClassName = utils.cTypeToJsName(field.type);

      // We need to find all of the libgit2 types to build the dependency chain
      var libgitType;

      libgit2.types.some(function (type) {
        if (type[0] === normalizedType) {
          libgitType = type[1];
          return true;
        }
      });

      if (libgitType) {
        dependencyLookup[libgitType.file] = "";

        field.isEnum = libgitType.type === "enum";
        field.hasConstructor
          = libgitType.used
            && libgitType.used.needs
            && libgitType.used.needs[1]
            && utils.isConstructorFunction(
              normalizedType,
              libgitType.used.needs[1]);
      }
    });

    Object.keys(dependencyLookup).forEach(function (dependencyFilename) {
      structDef.dependencies.push("../include/" + dependencyFilename);
    });
  }

  _.merge(structDef, structDefOverrides);

  if (!structDef.ignore) {
    structDef.fields = structDef.fields.filter(function (field) {
      return !field.ignore;
    });

    hashMap[structName] = structDef;
  }

  return hashMap;
}, {});

var classes = libgit2.groups.reduce(function(hashMap, current) {
  var className = current[0];
  var classDefOverrides = descriptor[className] || {};
  var classDef = {
    isClass: true,
    cType: "git_" + className,
    filename: className + ".h",
    cppClassName: utils.cTypeToCppName("git_" + className),
    jsClassName: utils.cTypeToJsName("git_" + className),
    functions: current[1].reduce(function(fnHashMap, currentFn) {
      fnHashMap[currentFn] = libgit2.functions[currentFn];
      return fnHashMap;
    }, {}),
    dependencies: [],
    fields: []
  };

  var dependencyLookup = {};

  // Decorate functions and calculate dependencies
  Object.keys(classDef.functions).forEach(function(key) {
    var fnDef = classDef.functions[key];

    // if this is the free function for the class, make the ref on the class
    if (key == classDef.cType + "_free") {
      classDef.freeFunctionName = key;
    }

    fnDef.cppFunctionName = utils.cTypeToCppName(key);
    fnDef.jsFunctionName = utils.cTypeToJsName(key);
    fnDef.isAsync = false; // until proven otherwise

    fnDef.args.forEach(function(arg) {
      var normalizedType = utils.normalizeCtype(arg.type);

      // We need to find all of the libgit2 types to build the dependency chain
      if (structs[normalizedType]) {
        dependencyLookup[structs[normalizedType].file] = "";
      }

      arg.cppClassName = utils.cTypeToCppName(normalizedType);
      arg.jsClassName = utils.cTypeToJsName(normalizedType);

      // Mark all of the args that are either returns or are the object
      // itself and determine if this function goes on the prototype
      // or is a constructor method.
      arg.isReturn = arg.name == "out"
        || (
          utils.isDoublePointer(arg.type)
          && normalizedType == classDef.cType);
      arg.isSelf
        = utils.isPointer(arg.type)
        && normalizedType == classDef.cType;

      if (arg.isReturn && classDef.return && classDef.return.type == int) {
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
  });

  Object.keys(dependencyLookup).forEach(function(dependencyFilename) {
    classDef.dependencies.push("../include/" + dependencyFilename);
  });

  _.merge(classDef, classDefOverrides);

  if (!classDef.ignore) {
    Object.keys(classDef.functions).forEach(function(key) {
      var fn = classDef.functions[key];

      if (fn.ignore) {
        classDef.functions[key] = undefined;
        delete classDef.functions[key];
      }
    });

    classDef.fields = classDef.fields.filter(function (field) {
      return !field.ignore;
    });

    hashMap[className] = classDef;
  }

  return hashMap;
}, {});

var output = [];

Object.keys(structs).forEach(function (key) {
  output.push(structs[key]);
});

Object.keys(classes).forEach(function (key) {
  output.push(classes[key]);
});

fs.writeFileSync(path.join(__dirname, "idefs.json"),
  JSON.stringify(output, null, 2));

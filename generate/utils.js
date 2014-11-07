var pointerRegex = /\s*\*\s*/;
var doublePointerRegex = /\s*\*\*\s*/;
var callbackFunctionNamePattern = /\s*_cb/;
var _ = require("lodash");

// TODO: When libgit2's docs include callbacks we should be able to remove this
var callbackDefs = require("./callbacks.json");
var descriptor = require("./descriptor.json");
var version = require("../package.json").libgit2.version;
var libgit2 = require("./v" + version + ".json");
var cTypes = libgit2.groups.map(function(group) { return group[0];});

var cTypeMappings = {
  "char": "String",
  "short": "Number",
  "int": "Number",
  "int16_t": "Number",
  "int32_t": "Number",
  "int64_t": "Number",
  "size_t": "Number",
  "uint16_t": "Number",
  "uint32_t": "Number",
  "uint64_t": "Number"
}

var Utils = {
  titleCase: function(str) {
    return str.split(/_|\//).map(function(val, index) {
      if (val.length) {
        return val[0].toUpperCase() + val.slice(1);
      }

      return val;
    }).join("");
  },

  camelCase: function(str) {
    return str.split(/_|\//).map(function(val, index) {
        return (index >= 1
          ? val[0].toUpperCase() + val.slice(1)
          : val[0].toLowerCase() + val.slice(1));
    }).join("");
  },

  isPointer: function(cType) {
    return pointerRegex.test(cType) || doublePointerRegex.test(cType);
  },

  isDoublePointer: function(cType) {
    return doublePointerRegex.test(cType);
  },

  normalizeCtype: function(cType) {
    return (cType || "")
    .toLowerCase()
    .replace("const ", "")
    .replace("unsigned ", "")
    .replace("struct", "")
    .replace(doublePointerRegex, "")
    .replace(pointerRegex, "")
    .trim();
  },

  cTypeToCppName: function(cType, ownerType) {
    var normalizedType = Utils.normalizeCtype(cType);
    if (ownerType && normalizedType != ownerType) {
      normalizedType = normalizedType.replace(ownerType, "");
    }

    return cTypeMappings[normalizedType] || Utils.titleCase(normalizedType);
  },

  cTypeToJsName: function(cType, ownerType) {
    return Utils.camelCase(Utils.cTypeToCppName(cType, ownerType).replace("Git", ""));
  },

  isConstructorFunction: function(cType, fnName) {
    var initFnName = cType.split('_');

    initFnName.splice(-1, 0, "init");
    initFnName = initFnName.join('_');

    return initFnName === fnName;
  },

  hasConstructor: function(type, normalizedType) {
    return type.used
      && type.used.needs
      && type.used.needs.some(function (fnName) {
        return Utils.isConstructorFunction(normalizedType, fnName);
      });
  },

  isCallbackFunction: function(fnName) {
    return callbackFunctionNamePattern.test(fnName);
  },

  isPayloadFor: function(cbName, payloadName) {
    return ~payloadName.indexOf("_payload")
      && Utils.isCallbackFunction(cbName)
      && ~cbName.indexOf(payloadName.replace("_payload", ""));
  },

  isClass: function(typeName, groupNames) {
    return !!~groupNames.indexOf(typeName);
  },

  getLibgitType: function(normalizedType, types) {
    var libgitType;

    types.some(function (type) {
      if (type[0] === normalizedType) {
        libgitType = type[1];
        return true;
      }
    });

    return libgitType;
  },

  processCallback: function(field) {
    field.isCallbackFunction = true;

    if (callbackDefs[field.type]) {
      _.merge(field, callbackDefs[field.type]);
    }
    else {
      console.log("WARNGING: Couldn't find callback definition for " + field.type);
    }
  },

  processPayload: function(field, allFields) {
    if (field.name === "payload") {
      field.payloadFor = "*";
    }
    else {
      var cbFieldName;

      allFields.some(function (cbField) {
        if (Utils.isPayloadFor(cbField.name, field.name)) {
          cbFieldName = cbField.name;
          return true;
        }
      });

      if (cbFieldName) {
        field.payloadFor = cbFieldName;
      }
    }
  },

  decorateLibgitType: function(type, types) {
    var normalizedType = Utils.normalizeCtype(type.cType);
    var libgitType = Utils.getLibgitType(normalizedType, types);

    if (libgitType) {
      type.isLibgitType = true;
      type.isEnum = libgitType.type === "enum";
      type.hasConstructor = Utils.hasConstructor(type, normalizedType);

      // there are no enums at the struct level currently, but we still need to override function args
      if (type.isEnum) {
        type.cppClassName = "Number";
        type.jsClassName = "Number";
      }

      _.merge(type, descriptor[normalizedType.replace("git_", "")] || {});
    }
  },

  decoratePrimaryType: function(typeDef) {
    var typeDefOverrides = descriptor[typeDef.typeName] || {};
    var partialOverrides = _.omit(typeDefOverrides, ["fields", "functions"]);

    typeDef.cType = typeDef.cType || null;
    typeDef.cppClassName = Utils.cTypeToCppName(typeDef.cType || "git_" + typeDef.typeName);
    typeDef.jsClassName = Utils.titleCase(Utils.cTypeToJsName(typeDef.cType || "git_" + typeDef.typeName));

    typeDef.dependencies = [];
    typeDef.filename = typeDef.typeName;

    typeDef.fields = typeDef.fields || [];

    typeDef.fields.forEach(function (field, index, allFields) {
      var fieldOverrides = typeDefOverrides.fields || {};
      Utils.decorateField(field, allFields, fieldOverrides[field.name] || {});
    });

    typeDef.needsForwardDeclaration = typeDef.decl === typeDef.cType;

    _.merge(typeDef, partialOverrides);
  },

  decorateStruct: function(structDef) {
    /* There are no enums for now, and when we get there I'm gonna argue they should have
        their own process
    if (structDef.type === "enum") {
      structDef.isEnum = true;
      structDef.cppClassName = "Number";
      structDef.jsClassName = "Number";
    } */

    structDef.isLibgitType = true;

    // TODO: Structs don't *have* functions, we need to remove the code thats requiring this (if there even is any)
    structDef.functions = [];

    var normalizedType = Utils.normalizeCtype(structDef.cType);
    structDef.hasConstructor = Utils.hasConstructor(structDef, normalizedType);
  },

  decorateClass: function(classDef) {
    var typeDefOverrides = descriptor[classDef.typeName] || {};

    classDef.functions = (classDef.functions || []).map(function(fn) {
      var fnDef = libgit2.functions[fn];
      fnDef.cFunctionName = fn;
      return fnDef;
    });

    var functionOverrides = typeDefOverrides.functions || {};
    classDef.functions.forEach(function(fnDef) {
      Utils.decorateFunction(fnDef, classDef, functionOverrides[fnDef.cFunctionName] || {});
    });
  },

  decorateField: function(field, allFields, fieldOverrides) {
    var normalizeType = Utils.normalizeCtype(field.type);

    field.cType = field.type;
    field.cppFunctionName = Utils.titleCase(field.name);
    field.jsFunctionName = Utils.camelCase(field.name);
    field.cppClassName = Utils.cTypeToCppName(field.type);
    field.jsClassName = Utils.titleCase(Utils.cTypeToJsName(field.type));

    if (Utils.isCallbackFunction(field.name)) {
      Utils.processCallback(field);
      var argOverrides = fieldOverrides.args || {};
      field.args.forEach(function (arg) {
        Utils.decorateArg(arg, null, null, argOverrides[arg.name] || {});
      });
    }
    else {
      field.isCallbackFunction = false;
      Utils.processPayload(field, allFields);
      if (field.payloadFor) {
        return;
      }
    }

    Utils.decorateLibgitType(field, libgit2.types);
    _.merge(field, fieldOverrides);
  },

  decorateArg: function(arg, classDef, fnDef, argOverrides) {
    var type = arg.cType || arg.type;
    var normalizedType = Utils.normalizeCtype(type);

    arg.cType = type;
    arg.cppClassName = Utils.cTypeToCppName(arg.cType);
    arg.jsClassName = Utils.titleCase(Utils.cTypeToJsName(arg.cType));

    Utils.decorateLibgitType(arg, libgit2.types);

    if (classDef && fnDef) {
      // Mark all of the args that are either returns or are the object
      // itself and determine if this function goes on the prototype
      // or is a constructor method.
      arg.isReturn = arg.name === "out" || (Utils.isDoublePointer(arg.type) && normalizedType == classDef.cType);
      arg.isSelf = Utils.isPointer(arg.type) && normalizedType == classDef.cType;

      if (arg.isReturn && fnDef.return && fnDef.return.type === "int") {
        fnDef.return.isErrorCode = true;
        fnDef.isAsync = true;
      }

      if (arg.isReturn && arg.isSelf) {
        arg.isSelf = false;
        fnDef.isConstructorMethod = true;
      }
      else if (arg.isSelf) {
        fnDef.isPrototypeMethod = true;
      }
    }

    _.merge(arg, argOverrides);
  },

  decorateFunction: function(fnDef, classDef, fnOverrides) {
    var key = fnDef.cFunctionName;

    // if this is the free function for the class, make the ref on the class
    // and then return since we don't want the free functions publicly
    // available
    if (key == classDef.cType + "_free") {
      classDef.freeFunctionName = key;
      fnDef.ignore = true;
      return;
    }

    fnDef.cppFunctionName = Utils.cTypeToCppName(key, "git_" + classDef.typeName);
    fnDef.jsFunctionName = Utils.cTypeToJsName(key, "git_" + classDef.typeName);
    //fnDef.isAsync = false; // until proven otherwise


    if (fnDef.cppFunctionName == classDef.cppClassName) {
      fnDef.cppFunctionName = fnDef.cppFunctionName.replace("Git", "");
    }

    var argOverrides = fnOverrides.args || {};
    fnDef.args.forEach(function(arg) {
      Utils.decorateArg(arg, classDef, fnDef, argOverrides[arg.name] || {});
    });

    if (fnDef.return) {
      Utils.decorateArg(fnDef.return, classDef, fnDef, fnOverrides.return || {});
    }

    _.merge(fnDef, _.omit(fnOverrides, "args", "return"));
  }
};

module.exports = Utils;

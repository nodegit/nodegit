var callbackTypePattern = /\s*_(cb|fn)/,
  selfReferentialTypePattern = /\s*_fn/;

var utils = require("./utils");
var _ = require("lodash");
var path = require("path");
var fs = require("fs");

// TODO: When libgit2's docs include callbacks we should be able to remove this
var callbackDefs = require("../input/callbacks.json");
var descriptor = require("../input/descriptor.json");
var libgit2 = require("../input/libgit2-docs.json");

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
  "uint64_t": "Number",
  "double": "Number",
  "git_object_size_t": "Number"
}

var collisionMappings = {
  "new": "create"
}

var Helpers = {
  normalizeCtype: function(cType) {
    return (cType || "")
    .toLowerCase()
    .replace("const ", "")
    .replace("unsigned ", "")
    .replace("struct", "")
    .replace(utils.doublePointerRegex, "")
    .replace(utils.pointerRegex, "")
    .replace(utils.arrayTypeRegex, "")
    .trim();
  },

  cTypeToCppName: function(cType, ownerType) {
    var normalizedType = Helpers.normalizeCtype(cType);
    if (ownerType && normalizedType != ownerType) {
      normalizedType = normalizedType.replace(ownerType, "");
    }

    return cTypeMappings[normalizedType] || utils.titleCase(normalizedType);
  },

  cTypeToJsName: function(cType, ownerType) {
    var output = utils.camelCase(Helpers.cTypeToCppName(cType, ownerType).replace(/^Git/, ""));
    var mergedPrefixes = ["from", "by"];

    mergedPrefixes.forEach(function(prefix) {
      var reg = new RegExp("(^" + prefix + "|" + utils.titleCase(prefix) + ")([a-z]+)$");
      output = output.replace(reg, function(all, prefixMatch, otherWord) {
        return prefixMatch + utils.titleCase(otherWord);
      });
    });

    output = output.replace(/([a-z])Str$/, "$1String")
    return output;
  },

  isConstructorFunction: function(cType, fnName) {
    var deprecatedInitFnName = cType.split("_");
    deprecatedInitFnName.splice(-1, 0, "init");
    deprecatedInitFnName = deprecatedInitFnName.join("_");

    var initFnName = cType + "_init";

    return initFnName === fnName || deprecatedInitFnName === fnName;
  },

  hasConstructor: function(type, normalizedType) {
    if (normalizedType && descriptor.types[normalizedType.substr(4)]) {
      var descriptorEntry = descriptor.types[normalizedType.substr(4)];
      if (descriptorEntry.hasOwnProperty('hasConstructor')) {
        return descriptorEntry.hasConstructor;
      }
    }

    return type.used
      && type.used.needs
      && type.used.needs.some(function (fnName) {
        return Helpers.isConstructorFunction(normalizedType, fnName);
      });
  },

  isCallbackFunction: function(cType, isCallback) {
    return isCallback === true || callbackTypePattern.test(cType);
  },

  isPayloadFor: function(cbField, payloadName) {
    return payloadName && ~payloadName.indexOf("_payload")
      && Helpers.isCallbackFunction(cbField.cType)
      && ~cbField.name.indexOf(payloadName.replace("_payload", ""));
  },

  isSelfReferential: function(cType){
    return selfReferentialTypePattern.test(cType);
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

    var callbackDef = callbackDefs[field.type] || callbackDefs[field.cType];
    if (callbackDef) {
      _.merge(field, callbackDef);
    }
    else {
      if (process.env.BUILD_ONLY) {
        console.warn("Couldn't find callback definition for " + field.type);
      }
    }
  },

  processPayload: function(field, allFields) {
    if (field.name === "payload") {
      field.payloadFor = "*";
      field.globalPayload = true;
      field.isOptional = true;
    }
    else {
      var cbFieldName;

      allFields.some(function (cbField) {
        if (Helpers.isPayloadFor(cbField, field.name)) {
          cbFieldName = cbField.name;
          return true;
        }
      });

      if (cbFieldName) {
        field.payloadFor = cbFieldName;
        field.isOptional = true;
      }
    }
  },

  // returns the libgittype found in types
  decorateLibgitType: function(type, types, enums) {
    var normalizedType = Helpers.normalizeCtype(type.cType);
    var libgitType = Helpers.getLibgitType(normalizedType, types);

    if (libgitType) {
      type.isLibgitType = true;
      type.isEnum = libgitType.type === "enum";
      type.hasConstructor = Helpers.hasConstructor(libgitType, normalizedType);
      type.isClassType = !type.isEnum && !type.hasConstructor;
      type.isStructType = !type.isEnum && !!type.hasConstructor;

      // there are no enums at the struct level currently, but we still need to override function args
      if (type.isEnum) {
        type.cppClassName = "Number";
        type.jsClassName = "Number";
        if (enums[type.cType]) {
          type.isMask = enums[type.cType].isMask || false
        }
      }

      type.freeFunctionName = libgitType.freeFunctionName;

      // we don't want to overwrite the c type of the passed in type
      _.merge(type, descriptor.types[normalizedType.replace("git_", "")] || {}, { cType: type.cType });
    }

    return libgitType;
  },

  decoratePrimaryType: function(typeDef, enums) {
    var typeDefOverrides = descriptor.types[typeDef.typeName] || {};
    var partialOverrides = _.omit(typeDefOverrides, ["fields", "functions"]);

    typeDef.cType = typeDef.cType || null;
    typeDef.cppClassName = Helpers.cTypeToCppName(typeDef.cType || "git_" + typeDef.typeName);
    typeDef.jsClassName = utils.titleCase(Helpers.cTypeToJsName(typeDef.cType || "git_" + typeDef.typeName));
    typeDef.filename = typeDef.typeName;
    typeDef.isLibgitType = true;
    typeDef.dependencies = [];
    typeDef.selfFreeing = Boolean(typeDefOverrides.selfFreeing);

    if (typeDefOverrides.freeFunctionName) {
      typeDef.freeFunctionName = typeDefOverrides.freeFunctionName;
    } else if (typeDef.type === 'struct') {
      typeDef.freeFunctionName = 'free';
    }

    typeDef.fields = typeDef.fields || [];
    typeDef.fields.forEach(function (field, index, allFields) {
      var fieldOverrides = typeDefOverrides.fields || {};
      Helpers.decorateField(field, allFields, fieldOverrides[field.name] || {}, enums);
    });

    typeDef.needsForwardDeclaration = typeDef.decl === typeDef.cType;

    var normalizedType = Helpers.normalizeCtype(typeDef.cType);
    typeDef.hasConstructor = Helpers.hasConstructor(typeDef, normalizedType);

    typeDef.functions = (typeDef.functions).map(function(fn) {
      var fnDef = libgit2.functions[fn];
      fnDef.cFunctionName = fn;
      return fnDef;
    });

    var typeDefOverrides = descriptor.types[typeDef.typeName] || {};
    var functionOverrides = typeDefOverrides.functions || {};
    typeDef.functions.forEach(function(fnDef) {
      Helpers.decorateFunction(fnDef, typeDef, functionOverrides[fnDef.cFunctionName] || {}, enums);
    });

    _.merge(typeDef, partialOverrides);
  },

  decorateField: function(field, allFields, fieldOverrides, enums) {
    var normalizeType = Helpers.normalizeCtype(field.type);

    field.cType = field.type;
    field.cppFunctionName = utils.titleCase(field.name);
    field.jsFunctionName = utils.camelCase(field.name);
    field.cppClassName = Helpers.cTypeToCppName(field.type);
    field.jsClassName = utils.titleCase(Helpers.cTypeToJsName(field.type));
    field.ownedByThis = true;

    if (Helpers.isCallbackFunction(field.cType, field.isCallback)) {
      Helpers.processCallback(field);

      var argOverrides = fieldOverrides.args || {};
      field.args = field.args || [];
      field.args.forEach(function (arg) {
        Helpers.decorateArg(arg, field.args, null, null, argOverrides[arg.name] || {}, enums);
      });
    }
    else {
      field.isCallbackFunction = false;
      Helpers.processPayload(field, allFields);
      if (field.payloadFor) {
        return;
      }
    }

    Helpers.decorateLibgitType(field, libgit2.types, enums);
    _.merge(field, fieldOverrides);
  },

  decorateArg: function(arg, allArgs, typeDef, fnDef, argOverrides, enums) {
    var type = argOverrides.cType || argOverrides.type || arg.cType || arg.type;
    var normalizedType = Helpers.normalizeCtype(type);

    arg.cType = type;
    arg.cppClassName = Helpers.cTypeToCppName(arg.cType);
    arg.jsClassName = utils.titleCase(Helpers.cTypeToJsName(arg.cType));

    Helpers.decorateLibgitType(arg, libgit2.types, enums);

    // Some arguments can be callbacks
    if (Helpers.isCallbackFunction(type)) {
      Helpers.processCallback(arg);

      var callBackArgOverrides = argOverrides.args || {};
      arg.args = arg.args || [];
      arg.args.forEach(function (argForCallback) {
        Helpers.decorateArg(argForCallback, arg.args, null, null, callBackArgOverrides[argForCallback.name] || {}, enums);
      });
    }
    else if (typeDef && fnDef) {
      Helpers.processPayload(arg, allArgs);
      if(arg.payloadFor) {
        return;
      }

      // Mark all of the args that are either returns or are the object
      // itself and determine if this function goes on the prototype
      // or is a constructor method.
      arg.isReturn = arg.name === "out" || (utils.isDoublePointer(arg.type) && normalizedType == typeDef.cType);
      if (typeof arg.isSelf == 'undefined') {
        arg.isSelf = utils.isPointer(arg.type) &&
          normalizedType == typeDef.cType &&
          arg.cppClassName !== "Array" &&
          argOverrides.cppClassName !== "Array" &&
          _.every(allArgs, function(_arg) { return !_arg.isSelf; });
      }
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

  decorateFunction: function(fnDef, typeDef, fnOverrides, enums) {
    var key = fnDef.cFunctionName;

    // if this is the free function for the class, make the ref on the class
    // and then return since we don't want the free functions publicly
    // available
    if (key == typeDef.cType + "_free") {
      typeDef.freeFunctionName = key;
    }

    fnDef.cppFunctionName = Helpers.cTypeToCppName(key, "git_" + typeDef.typeName);
    fnDef.jsFunctionName = Helpers.cTypeToJsName(key, "git_" + typeDef.typeName);
    fnDef.jsClassName = typeDef.jsClassName;

    if (fnDef.cppFunctionName == typeDef.cppClassName) {
      fnDef.cppFunctionName = fnDef.cppFunctionName.replace("Git", "");
    }

    var argOverrides = fnOverrides.args || {};
    // loop over all the argument names explicitly written in descriptor.json
    Object.keys(argOverrides).forEach(function(argOverride) {
      // see if the argument names match a name in the C function
      var found = fnDef.args.some(function(arg) {
        return arg.name === argOverride;
      });
      if (!found) {
        console.log(
          "Found non-matching argument named %s in descriptor.json for C function %s",
          argOverride,
          fnDef.cFunctionName
        );
      }
    });
    fnDef.args.forEach(function(arg) {
      Helpers.decorateArg(arg, fnDef.args, typeDef, fnDef, argOverrides[arg.name] || {}, enums);

      // if a function has any callbacks then it MUST be async
      if (arg.isCallbackFunction) {
        fnDef.isAsync = true;
      }
    });

    if (fnDef.cFile) {
      fnDef.implementation = fs.readFileSync(path.resolve(fnDef.cFile), 'utf8');
    }

    if (fnDef.return) {
      Helpers.decorateArg(fnDef.return, fnDef.args, typeDef, fnDef, fnOverrides.return || {}, enums);
    }

    _(collisionMappings).forEach(function(newName, collidingName) {
      if (fnDef.cppFunctionName == utils.titleCase(collidingName)) {
        fnDef.cppFunctionName = utils.titleCase(newName);
      }

      if (fnDef.jsFunctionName == utils.camelCase(collidingName)) {
        fnDef.jsFunctionName = utils.camelCase(newName);
      }
    });

    if ("git_" + typeDef.typeName == fnDef.cFunctionName) {
      fnDef.useAsOnRootProto = true;
    }
    _.merge(fnDef, _.omit(fnOverrides, "args", "return"));
  },

  filterIgnored: function (arr, callback) {
    if (!arr) {
      return;
    }
    for (var i = arr.length - 1; i >= 0; i--) {
      if (arr[i].ignore) {
        arr.splice(i, 1);
      }
      else if (callback) {
        callback(arr[i]);
      }
    }
  },

  deleteProperties: function(obj) {
    delete obj.line;
    delete obj.lineto;
    delete obj.block;
    delete obj.description;
    delete obj.comments;
    delete obj.tdef;
    delete obj.decl;
    delete obj.comments;
    delete obj.argline;
    delete obj.sig;
  },

  filterDocumentation: function(idefs) {
    Helpers.filterIgnored(idefs, function (idef) {
      Helpers.deleteProperties(idef);

      Helpers.filterIgnored(idef.fields, Helpers.deleteProperties);


      Helpers.filterIgnored(idef.functions, function (fn) {
        Helpers.deleteProperties(fn);

        Helpers.filterIgnored(fn.args, function(arg) {
          Helpers.deleteProperties(arg);
          delete arg.functions;
        });
      });
    });
  }
};

module.exports = Helpers;

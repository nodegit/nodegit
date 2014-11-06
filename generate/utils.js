var pointerRegex = /\s*\*\s*/;
var doublePointerRegex = /\s*\*\*\s*/;
var callbackFunctionNamePattern = /\s*_cb/;
var _ = require("lodash");

// TODO: When libgit2's docs include callbacks we should be able to remove this
var callbackDefs = require("./callbacks.json");

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
    return str.toLowerCase().split(/_|\//).map(function(val, index) {
      if (val.length) {
        return val[0].toUpperCase() + val.slice(1);
      }

      return val;
    }).join("");
  },

  camelCase: function(str) {
    return str.toLowerCase().split(/_|\//).map(function(val, index) {
      if (val.length) {
        return index >= 1 ? val[0].toUpperCase() + val.slice(1) : val;
      }

      return val;
    }).join("");
  },

  isPointer: function(cType) {
    return pointerRegex.test(cType);
  },

  isDoublePointer: function(cType) {
    return doublePointerRegex.test(cType);
  },

  normalizeCtype: function(cType) {
    return cType
    .toLowerCase()
    .replace("const ", "")
    .replace("unsigned ", "")
    .replace("struct", "")
    .replace(doublePointerRegex, "")
    .replace(pointerRegex, "")
    .trim();
  },

  cTypeToCppName: function(cType) {
    var normalizedType = Utils.normalizeCtype(cType);

    return cTypeMappings[normalizedType] || Utils.titleCase(normalizedType);
  },

  cTypeToJsName: function(cType) {
    return Utils.cTypeToCppName(cType).replace("Git", "");
  },

  isConstructorFunction: function(cType, fnName) {
    var initFnName = cType.split('_');

    initFnName.splice(-1, 0, "init");
    initFnName = initFnName.join('_');

    return initFnName === fnName;
  },

  isCallbackFunction: function(fnName) {
    return callbackFunctionNamePattern.test(fnName);
  },

  isPayloadFor: function(cbName, payloadName) {
    return ~payloadName.indexOf("_payload")
      && Utils.isCallbackFunction(cbName)
      && ~cbName.indexOf(payloadName.replace("_payload", ""));
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

  decorateLibgitType: function(type, types) {
    var normalizedType = Utils.normalizeCtype(type.cType);
    var libgitType = Utils.getLibgitType(normalizedType, types);

    if (libgitType) {
      type.isLibgitType = true;
      type.isEnum = libgitType.type === "enum";
      type.hasConstructor
        = libgitType.used
          && libgitType.used.needs
          && libgitType.used.needs.some(function (fnName) {
            return Utils.isConstructorFunction(normalizedType, fnName);
          });

      if (type.isEnum) {
        type.cppClassName = "Number";
        type.jsClassName = "Number";
      }
    }
  },

  decorateStruct: function(struct) {
    struct.cppClassName = Utils.cTypeToCppName(struct.cType);
    struct.jsClassName = Utils.cTypeToCppName(struct.cType);
    struct.filename = struct.cType.replace("git_", "");

    if (struct.type === "enum") {
      struct.isEnum = true;
      struct.cppClassName = "Number";
      struct.jsClassName = "Number";
    }

    struct.isForwardDeclared = struct.decl === struct.cType;
    struct.isLibgitType = true;
    struct.fields = struct.fields || [];
    struct.functions = [];
    struct.dependencies = [];
    struct.hasConstructor
      = struct.used
      && struct.used.needs
      && struct.used.needs.some(function (fnName) {
        return Utils.isConstructorFunction(struct.cType, fnName);
      });
  },

  decorateClass: function(className, classDef, classFunctions, libgit2) {
    classDef.isClass = true;
    classDef.filename = className;
    classDef.cppClassName = Utils.cTypeToCppName("git_" + className);
    classDef.jsClassName = Utils.cTypeToJsName("git_" + className);
    classDef.functions = classFunctions.reduce(function(fnArray, currentFn) {
        var fnDef = libgit2.functions[currentFn];
        fnDef.cFunctionName = currentFn;
        fnArray.push(fnDef);
        return fnArray;
      }, []);
    classDef.dependencies = [];
    classDef.fields = [];
    classDef.ignore = false;

    var cType = "git_" + className;
    var hasCtype = libgit2.types.some(function (type) {
      if (type[0] == cType) {
        return true;
      }
    });

    cType = hasCtype ? cType : null;

    classDef.cType = cType;

    classDef.functions.forEach(function(fnDef) {
      Utils.decorateFunction(fnDef, libgit2, classDef);
    });
  },

  decorateField: function(field, allFields, libgit2) {
    var normalizeType = Utils.normalizeCtype(field.type);

    field.cType = field.type;
    field.cppFunctionName = Utils.titleCase(field.name);
    field.jsFunctionName = Utils.camelCase(field.name);
    field.cppClassName = Utils.cTypeToCppName(field.type);
    field.jsClassName = Utils.cTypeToJsName(field.type);

    if (Utils.isCallbackFunction(field.name)) {
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
    }

    if (field.payloadFor) {
      return;
    }

    if (field.isCallbackFunction) {
      field.args.forEach(function (arg) {
        Utils.decorateArg(arg, libgit2);
      });
    }

    Utils.decorateLibgitType(field, libgit2.types);
  },

  decorateArg: function(arg, libgit2, classDef, fnDef) {
    var type = arg.cType || arg.type;
    var normalizedType = Utils.normalizeCtype(type);

    arg.cType = type;
    arg.cppClassName = Utils.cTypeToCppName(arg.cType);
    arg.jsClassName = Utils.cTypeToJsName(arg.cType);

    Utils.decorateLibgitType(arg, libgit2.types);

    if (!classDef || !fnDef) {
      return;
    }

    // Mark all of the args that are either returns or are the object
    // itself and determine if this function goes on the prototype
    // or is a constructor method.
    arg.isReturn
      = arg.name === "out"
      || (Utils.isDoublePointer(arg.type)
        && normalizedType == classDef.cType);
    arg.isSelf
      = Utils.isPointer(arg.type)
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
  },

  decorateFunction: function(fnDef, libgit2, classDef) {
    var key = fnDef.cFunctionName;

    // if this is the free function for the class, make the ref on the class
    // and then return since we don't want the free functions publicly
    // available
    if (key == classDef.cType + "_free") {
      classDef.freeFunctionName = key;
      fnDef.ignore = true;
      return;
    }

    fnDef.cppFunctionName = Utils.cTypeToCppName(key);
    fnDef.jsFunctionName = Utils.cTypeToJsName(key);
    fnDef.isAsync = false; // until proven otherwise

    if (fnDef.cppFunctionName == classDef.cppClassName) {
      fnDef.cppFunctionName = fnDef.cppFunctionName.replace("Git", "");
    }

    fnDef.args.forEach(function(arg) {
      Utils.decorateArg(arg, libgit2, classDef, fnDef);
    });

    if (fnDef.return) {
      Utils.decorateArg(fnDef.return, libgit2);
    }
  }
};

module.exports = Utils;

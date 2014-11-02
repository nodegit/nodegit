var pointerRegex = /\s*\*\s*/;
var doublePointerRegex = /\s*\*\*\s*/;
var callbackFunctionNamePattern = /\s*_cb/;

var cTypeMappings = {
  "char": "String",
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
  }
};

module.exports = Utils;

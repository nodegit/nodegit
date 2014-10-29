var pointerRegex = /\s*\*\s*/;
var doublePointerRegex = /\s*\*\*\s*/;

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
    .replace("const ", "")
    .replace(doublePointerRegex, "")
    .replace(pointerRegex, "")
    .trim();
  },

  cTypeToCppClassName: function(cType) {
    return Utils.titleCase(Utils.normalizeCtype(cType));
  },

  cTypeToJsClassName: function(cType) {
    return Utils.cTypeToCppClassName(cType).replace("Git", "");
  }
};

module.export = Utils;

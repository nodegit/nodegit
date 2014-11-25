const promisify = require("promisify-node");
const fse = require("fs-extra");

const fs = require("fs");
const path = require("path");

// Make a locally bound path joiner, (bound to generate).
var local = path.join.bind(null, __dirname, "../");

var util = {
  pointerRegex: /\s*\*\s*/,
  doublePointerRegex: /\s*\*\*\s*/,

  readFile: function(file) {
    try {
      return fs.readFileSync(local(file)).toString();
    }
    catch (unhandledException) {
      return "";
    }
  },

  writeFile: function(file, content) {
    try {
      var file = local(file);
      if (typeof content == "object") {
        content = JSON.stringify(content, null, 2)
      }

      fse.ensureFileSync(file);
      fse.writeFileSync(file, content);
      return true;
    }
    catch (exception) {
      return false;
    }
  },

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

  isPointer: function(type) {
    return util.pointerRegex.test(type) || util.doublePointerRegex.test(type);
  },

  isDoublePointer: function(type) {
    return util.doublePointerRegex.test(type);
  }

};

module.exports = util;

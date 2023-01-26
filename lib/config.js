var util = require("util");
var NodeGit = require("../");

var Config = NodeGit.Config;

var _getBool = Config.prototype.getBool;
var _setBool = Config.prototype.setBool;

/**
 * @param {string} name  The variable's name.
 * @returns {boolean} The. Variable's value.
 * @async
 */
Config.prototype.getBool = function (name) {
  return _getBool.call(this, name).then((result) => Boolean(result));
};

/**
 * @param {string}  name  The variable's name.
 * @param {boolean} name  The variable's value.
 * @returns {number} 0. Or an error code.
 * @async
 */
Config.prototype.setBool = function (name, value) {
  return _setBool.call(this, name, value ? 1 : 0);
};

// Backwards compatibility.
Config.prototype.getString = function () {
  return this.getStringBuf.apply(this, arguments);
};

NodeGit.Enums.CVAR = {};
var DEPRECATED_CVAR_ENUMS = ["FALSE", "TRUE", "INT32", "STRING"];
DEPRECATED_CVAR_ENUMS.forEach((key) => {
  Object.defineProperty(NodeGit.Enums.CVAR, key, {
    get: util.deprecate(() => Config.MAP[key], `Use NodeGit.Config.MAP.${key} instead of NodeGit.Enums.CVAR.${key}.`),
  });
});

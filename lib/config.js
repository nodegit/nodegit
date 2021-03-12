var util = require("util");
var NodeGit = require("../");

var Config = NodeGit.Config;

var _getBool = Config.prototype.getBool;
var _setBool = Config.prototype.setBool;

/**
 * @async
 * @param {String} name The variable's name
 * @return {Boolean} The variable's value
 */
Config.prototype.getBool = function(name) {
  return _getBool.call(this, name)
    .then(result => Boolean(result));
};

/**
 * @async
 * @param {String} name The variable's name
 * @param {Boolean} name The variable's value
 * @return {Number} 0 or an error code
 */
Config.prototype.setBool = function(name, value) {
  return _setBool.call(this, name, value ? 1 : 0);
};

// Backwards compatibility.
Config.prototype.getString = function() {
  return this.getStringBuf.apply(this, arguments);
};

NodeGit.Enums.CVAR = {};
var DEPRECATED_CVAR_ENUMS = [
  "FALSE",
  "TRUE",
  "INT32",
  "STRING"
];
DEPRECATED_CVAR_ENUMS.forEach((key) => {
  Object.defineProperty(NodeGit.Enums.CVAR, key, {
    get: util.deprecate(
      () => Config.MAP[key],
      `Use NodeGit.Config.MAP.${key} instead of NodeGit.Enums.CVAR.${key}.`
    )
  });
});

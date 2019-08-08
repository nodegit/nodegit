var util = require("util");
var NodeGit = require("../");

var Config = NodeGit.Config;

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

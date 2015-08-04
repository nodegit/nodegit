var NodeGit = require("../");

var Config = NodeGit.Config;

// Backwards compatibility.
Config.getString = function() {
  return Config.getStringBuf.apply(this, arguments);
};

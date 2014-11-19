var NodeGit = require("../");

var OdbObject = NodeGit.OdbObject;

OdbObject.prototype.toString = function(size) {
  size = size || this.size();

  return this.data().toBuffer(size).toString();
};

module.exports = OdbObject;

var util = require("util");
var NodeGit = require("../");
var Buf = NodeGit.Buf;

var _set = Buf.prototype.set;
var _grow = Buf.prototype.grow;
var _isBinary = Buf.prototype.isBinary;
var _containsNul = Buf.prototype.containsNul;

/**
 * Sets the content of a GitBuf to a string.
 * @param {string} The utf8 value to set in the buffer.
 *                 The string will be null terminated.
 */
var _setString = function(content) {
  const buf = Buffer.from(content + "\0", "utf8");
  this.set(buf, buf.length);
};

Buf.prototype.set = util.deprecate(
  _set,
  "NodeGit.Buf.prototype.set is deprecated."
);

Buf.prototype.setString = util.deprecate(
  _setString,
  "NodeGit.Buf.prototype.setString is deprecated."
);

Buf.prototype.grow = util.deprecate(
  _grow,
  "NodeGit.Buf.prototype.grow is deprecated."
);

Buf.prototype.isBinary = util.deprecate(
  _isBinary,
  "NodeGit.Buf.prototype.isBinary is deprecated."
);

Buf.prototype.containsNul = util.deprecate(
  _containsNul,
  "NodeGit.Buf.prototype.containsNul is deprecated."
);
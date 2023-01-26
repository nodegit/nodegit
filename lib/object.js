var util = require("util");
var NodeGit = require("../");

var Obj = NodeGit.Object;

/**
 * Is this object a blob?
 *
 * @returns {boolean}
 */
Obj.prototype.isBlob = function () {
  return this.type() == Obj.TYPE.BLOB;
};

/**
 * Is this object a commit?
 *
 * @returns {boolean}
 */
Obj.prototype.isCommit = function () {
  return this.type() == Obj.TYPE.COMMIT;
};

/**
 * Is this object a tag?
 *
 * @returns {boolean}
 */
Obj.prototype.isTag = function () {
  return this.type() == Obj.TYPE.TAG;
};

/**
 * Is this object a tree?
 *
 * @returns {boolean}
 */
Obj.prototype.isTree = function () {
  return this.type() == Obj.TYPE.TREE;
};

// Deprecated -----------------------------------------------------------------

Object.defineProperty(Obj.TYPE, "BAD", {
  get: util.deprecate(() => Obj.TYPE.INVALID, "Use NodeGit.Object.TYPE.INVALID instead of NodeGit.Object.TYPE.BAD."),
});

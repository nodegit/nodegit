var git = require('../'),
    Object = git.Object;

Object.Type = {
  Any: -2,               /**< Object can be any of the following */
  Bad: -1,               /**< Object is invalid. */
  Ext1: 0,              /**< Reserved for future use. */
  Commit: 1,             /**< A commit object. */
  Tree: 2,               /**< A tree (directory listing) object. */
  Blob: 3,               /**< A file revision object. */
  Tag: 4,                /**< An annotated tag object. */
  Ext2: 5,              /**< Reserved for future use. */
  OffsetDelta: 6, /**< A delta, base is given by an offset. */
  OidDelta: 7 /**< A delta, base is given by object id. */
};

Object.prototype.isCommit = function() {
  return this.type() == Object.Type.Commit;
};

Object.prototype.isTree = function() {
  return this.type() == Object.Type.Tree;
};

Object.prototype.isBlob = function() {
  return this.type() == Object.Type.Blob;
};

Object.prototype.isTag = function() {
  return this.type() == Object.Type.Tag;
};

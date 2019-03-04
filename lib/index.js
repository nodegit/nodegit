var util = require("util");
var NodeGit = require("../");

var Index = NodeGit.Index;

var _addAll = Index.prototype.addAll;
var _removeAll = Index.prototype.removeAll;
var _updateAll = Index.prototype.updateAll;

Index.prototype.addAll = function(pathspec, flags, matchedCallback) {
  return _addAll.call(this, pathspec || "*", flags, matchedCallback, null);
};

/**
 * Return an array of the entries in this index.
 * @return {Array<IndexEntry>} an array of IndexEntrys
 */
Index.prototype.entries = function() {
  var size = this.entryCount();
  var result = [];

  for (var i = 0; i < size; i++) {
    result.push(this.getByIndex(i));
  }

  return result;
};

Index.prototype.removeAll = function(pathspec, matchedCallback) {
  return _removeAll.call(this, pathspec || "*", matchedCallback, null);
};

Index.prototype.updateAll = function(pathspec, matchedCallback) {
  return _updateAll.call(this, pathspec || "*", matchedCallback, null);
};

// Deprecated -----------------------------------------------------------------

NodeGit.Index.CAP = {};
Object.keys(NodeGit.Index.CAPABILITY).forEach((key) => {
  Object.defineProperty(NodeGit.Index.CAP, key, {
    get: util.deprecate(
      () => NodeGit.Index.CAPABILITY[key],
      `Use NodeGit.Index.CAPABILITY.${key} instead of ` +
      `NodeGit.Index.CAP.${key}.`
    )
  });
});

NodeGit.Enums.INDXENTRY_FLAG = {};
Object.defineProperty(NodeGit.Enums.INDXENTRY_FLAG, "IDXENTRY_EXTENDED", {
  get: util.deprecate(
    () => NodeGit.Index.ENTRY_FLAG.ENTRY_EXTENDED,
    "Use NodeGit.Index.ENTRY_FLAG.ENTRY_EXTENDED instead of " +
    "NodeGit.Enums.INDXENTRY_FLAG.IDXENTRY_EXTENDED."
  )
});
Object.defineProperty(NodeGit.Enums.INDXENTRY_FLAG, "IDXENTRY_VALID", {
  get: util.deprecate(
    () => NodeGit.Index.ENTRY_FLAG.ENTRY_VALID,
    "Use NodeGit.Index.ENTRY_FLAG.ENTRY_VALID instead of " +
    "NodeGit.Enums.INDXENTRY_FLAG.IDXENTRY_VALID."
  )
});

NodeGit.Enums.IDXENTRY_EXTENDED_FLAG = {};
var EXTENDED_FLAGS_MAP = {
  IDXENTRY_INTENT_TO_ADD: "ENTRY_INTENT_TO_ADD",
  IDXENTRY_SKIP_WORKTREE: "ENTRY_SKIP_WORKTREE",
  S: "S",
  IDXENTRY_UPTODATE: "ENTRY_UPTODATE"
};
Object.keys(EXTENDED_FLAGS_MAP).forEach((key) => {
  const newKey = EXTENDED_FLAGS_MAP[key];
  Object.defineProperty(NodeGit.Enums.IDXENTRY_EXTENDED_FLAG, key, {
    get: util.deprecate(
      () => NodeGit.Index.ENTRY_EXTENDED_FLAG[newKey],
      `Use NodeGit.Index.ENTRY_EXTENDED_FLAG.${newKey} instead of ` +
      `NodeGit.Enums.IDXENTRY_EXTENDED_FLAG.${key}.`
    )
  });
});

var DEPRECATED_EXTENDED_FLAGS = {
  IDXENTRY_EXTENDED2: 32768,
  IDXENTRY_UPDATE: 1,
  IDXENTRY_REMOVE: 2,
  IDXENTRY_ADDED: 8,
  IDXENTRY_HASHED: 16,
  IDXENTRY_UNHASHED: 32,
  IDXENTRY_WT_REMOVE: 64,
  IDXENTRY_CONFLICTED: 128,
  IDXENTRY_UNPACKED: 256,
  IDXENTRY_NEW_SKIP_WORKTREE: 512,
};
Object.keys(DEPRECATED_EXTENDED_FLAGS).forEach((key) => {
  Object.defineProperty(NodeGit.Enums.IDXENTRY_EXTENDED_FLAG, key, {
    get: util.deprecate(
      () => DEPRECATED_EXTENDED_FLAGS[key],
      "LibGit2 has removed this flag for public usage."
    )
  });
});

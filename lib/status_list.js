var NodeGit = require("../");
var normalizeOptions = NodeGit.Utils.normalizeOptions;

var StatusList = NodeGit.StatusList;

var _create = StatusList.create;

// Override StatusList.create to normalize opts
StatusList.create = function(repo, opts) {
  opts = normalizeOptions(opts, NodeGit.StatusOptions);
  return _create(repo, opts);
};

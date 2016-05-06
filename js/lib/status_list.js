var NodeGit = require("../");
var normalizeOptions = NodeGit.Utils.normalizeOptions;

var StatusList = NodeGit.StatusList;

// Override StatusList.create to normalize opts
var create = StatusList.create;
StatusList.create = function(repo, opts) {
  opts = normalizeOptions(opts, NodeGit.StatusOptions);
  return create(repo, opts);
};

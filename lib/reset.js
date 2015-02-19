var NodeGit = require("../");
var normalizeOptions = require("./util/normalize_options");

var Reset = NodeGit.Reset;

var defaultFn = Reset.default;
Reset.default = function(repo, target, pathspecs) {
  return defaultFn.call(this, repo, target, pathspecs);
};

module.exports = Reset;

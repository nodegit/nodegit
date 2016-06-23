var fs = require("fs");
var path = require("path");

var isGitRepo;

try {
  fs.statSync(path.join(__dirname, "..", ".git"));
  isGitRepo = true;
} catch (e) {
  isGitRepo = false;
}

module.exports = {
  debugBuild: process.env.BUILD_DEBUG,
  isGitRepo: isGitRepo,
  mustBuild: isGitRepo || process.env.BUILD_DEBUG || process.env.BUILD_ONLY,
};

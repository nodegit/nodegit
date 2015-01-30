var nodegit = require("../");
var promisify = require("promisify-node");
var fse = promisify(require("fs-extra"));
var path = "/tmp/nodegit-github-2factor-demo";

var token = "{Your GitHub user token}";
var repoOwner = "{The orgname or username that owns the repo}";
var repoName = "{The name of the repo}";

// The token has to be included in the URL if the repo is private.
// Otherwise, github just wont respond, so a normal credential callback
// wont work.
var repoUrl = "https://" + token +
  ":-oauth-basic@github.com/" +
  repoOwner + "/" +
  repoName + ".git";

var opts = { ignoreCertErrors: 1};

// If the repo is public, you can use a callback instead
var repoUrl = "https://github.com/" + repoOwner + "/" + repoName + ".git";

var opts = {
  ignoreCertErrors: 1,
  remoteCallbacks: {
    credentials: function() {
      return NodeGit.Cred.userpassPlaintextNew (token, "x-oauth-basic");
    }
  }
}

fse.remove(path).then(function() {
  var entry;

  nodegit.Clone.clone(repoUrl, path, opts)
    .done(function(repo) {
      if (repo instanceof nodegit.Repository) {
        console.info("We cloned the repo!");
      }
      else {
        console.error("Something borked :(");
      }
    });
});

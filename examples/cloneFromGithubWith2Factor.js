var nodegit = require("../");
var promisify = require("thenify-all");
var fse = promisify(require("fs-extra"), ["remove"]);
var path = "/tmp/nodegit-github-2factor-demo";

var token = "{Your GitHub user token}";
var repoOwner = "{The orgname or username that owns the repo}";
var repoName = "{The name of the repo}";

// To clone with 2 factor auth enabled, you have to use a github oauth token
// over https, it can't be done with actual 2 factor.
// https://github.com/blog/1270-easier-builds-and-deployments-using-git-over-https-and-oauth

// The token has to be included in the URL if the repo is private.
// Otherwise, github just wont respond, so a normal credential callback
// wont work.
var repoUrl = "https://" + token +
  ":-oauth-basic@github.com/" +
  repoOwner + "/" +
  repoName + ".git";

// If the repo is public, you can use a callback instead
var repoUrl = "https://github.com/" + repoOwner + "/" + repoName + ".git";

var opts = {
  remoteCallbacks: {
    credentials: function() {
      return nodegit.Cred.userpassPlaintextNew (token, "x-oauth-basic");
    },
    certificateCheck: function() {
      return 1;
    }
  }
};

fse.remove(path).then(function() {
  nodegit.Clone(repoUrl, path, opts)
    .then(function(repo) {
      if (repo instanceof nodegit.Repository) {
        console.info("We cloned the repo!");
      }
      else {
        console.error("Something borked :(");
      }
    });
});

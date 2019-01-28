var nodegit = require("../");
var fse = require("fs-extra");
var path = "/tmp/nodegit-github-2factor-demo";

var token = "{Your GitHub user token}";
var repoOwner = "{The orgname or username that owns the repo}";
var repoName = "{The name of the repo}";

// To clone with 2 factor auth enabled, you have to use a github oauth token
// over https, it can't be done with actual 2 factor.
// https://github.com/blog/1270-easier-builds-and-deployments-using-git-over-https-and-oauth

// If the repo is public, you can use a callback instead
var repoUrl = "https://github.com/" + repoOwner + "/" + repoName + ".git";

var opts = {
  fetchOpts: {
    callbacks: {
      credentials: function() {
        return nodegit.Cred.userpassPlaintextNew(token, "x-oauth-basic");
      },
      certificateCheck: function() {
        return 0;
      }
    }
  }
};

fse.remove(path).then(function() {
  nodegit.Clone(repoUrl, path, opts)
    .done(function(repo) {
      if (repo instanceof nodegit.Repository) {
        console.info("We cloned the repo!");
      }
      else {
        console.error("Something borked :(");
      }
    });
});

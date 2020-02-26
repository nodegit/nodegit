// Require in NodeGit, since we want to use the local copy, we"re using a
// relative path.  In your project, you will use:
//
// var NodeGit = require("nodegit");
var NodeGit = require("../../../");

// To clone with two factor auth enabled, you have to use a GitHub OAuth token
// over HTTPS.
var GITHUB_TOKEN = "<GH_TOKEN>";

// Using the `clone` method from the `Git.Clone` module, bring down the NodeGit
// test repository from GitHub.
var cloneURL = "https://github.com/nodegit/private";

// Ensure that the `tmp` directory is local to this file and not the CWD.
var localPath = require("path").join(__dirname, "tmp");

// Simple object to store clone options.
var cloneOptions = {};

// This is a required callback for OS X machines.  There is a known issue
// with libgit2 being able to verify certificates from GitHub.
cloneOptions.fetchOpts = {
  callbacks: {
    certificateCheck: function() { return 0; },
    credentials: function() {
      return NodeGit.Credential.userpassPlaintextNew(GITHUB_TOKEN, "x-oauth-basic");
    }
  }
};

// Invoke the clone operation and store the returned Promise.
var cloneRepository = NodeGit.Clone(cloneURL, localPath, cloneOptions);

// If the repository already exists, the clone above will fail.  You can simply
// open the repository in this case to continue execution.
var errorAndAttemptOpen = function() {
  return NodeGit.Repository.open(localPath);
};

// Once the repository has been cloned or opened, you can work with a returned
// `Git.Repository` instance.
cloneRepository.catch(errorAndAttemptOpen)
  .then(function(repository) {
    // Access any repository methods here.
    console.log("Is the repository bare? %s", Boolean(repository.isBare()));
  });

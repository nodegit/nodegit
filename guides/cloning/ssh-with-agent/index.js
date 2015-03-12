// Require in NodeGit, since we want to use the local copy, we're using a
// relative path.  In your project, you will use:
//
// var NodeGit = require("nodegit");
var NodeGit = require("../../../");

// Set the URL that NodeGit will connect to clone.
var cloneURL = "git@github.com:nodegit/test";

// Ensure that the `tmp` directory is local to this file and not the CWD.
var localPath = require("path").join(__dirname, "tmp");

// Simple object to store clone options.
var cloneOptions = {};

// This is a required callback for OS X machines.  There is a known issue
// with libgit2 being able to verify certificates from GitHub.
cloneOptions.remoteCallbacks = {
  certificateCheck: function() { return 1; },

  // Credentials are passed two arguments, url and username. We forward the
  // `userName` argument to the `sshKeyFromAgent` function to validate
  // authentication.
  credentials: function(url, userName) {
    return NodeGit.Cred.sshKeyFromAgent(userName);
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

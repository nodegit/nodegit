// Require in NodeGit, since we want to use the local copy, we're using a
// relative path.  In your project, you will use:
//
// var NodeGit = require("nodegit");
var NodeGit = require("../../../");

// Using the `init` method from the `NodeGit.Repository` module, we can create
// a blank repository using NodeGit
var pathToRepo = require("path").resolve("../my-git-projects/my-project");

// Setting this to 0 will have the effect creating a `.git` folder inside of
// passed path. If this is 1 then we will use the passed path AS our `.git`
// folder.
var isBare = 0;

// In NodeGit we use Promises to make callbacks easier to deal with.
//
// For more information visit https://www.promisejs.org/
NodeGit.Repository.init(pathToRepo, isBare).then(function (repo) {
  // In this function we have a repo object that we can perform git operations
  // on.

  // Note that with a new repository many functions will fail until there is
  // an initial commit.
})
.catch(function (reasonForFailure) {
  // If the repo cannot be created for any reason we can handle that case here.
  // NodeGit won't init a repo over a pre-existing repo.
});

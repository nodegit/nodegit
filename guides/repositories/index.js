// Require in NodeGit, since we want to use the local copy, we're using a
// relative path.  In your project, you will use:
//
// var NodeGit = require("nodegit");
var NodeGit = require("../../../");

// Using the `open` method from the `NodeGit.Repository` module, we can open
// a repository using NodeGit
var pathToRepo = require("path").resolve("../my-git-projects/my-project");

// In NodeGit we use Promises to make callbacks easier to deal with.
//
// For more information visit https://www.promisejs.org/
NodeGit.Repository.open(pathToRepo).then(function (repo) {
  // In this function we have a repo object that we can perform git operations
  // on.
  // NOTE: Many NodeGit objects will appear as empty objects if inspected in
  // the console. This is a known issue. You can track it's progress at
  // https://github.com/nodegit/nodegit/issues/307
})
// Promises will swallow errors and not report them unless you have supplied
// a second function to the `.then` or end the chain with either a `.catch` or
// a `.done`
.then(function (successfulResult) {
  // This is the first function of the then which contains the successfully
  // calculated result of the promise
}, function (reasonForFailure) {
  // This is the second function of the then which contains the reason the
  // promise failed
})
.catch(function (reasonForFailure) {
  // You can also provide a catch function which will contain the reason why
  // any above promises that weren't handled have failed
})
.done(function() {
  // If we have a .done then the error will be thrown if there was an error that
  // wasn't caught by either providing a 2nd function to the `.then` or a
  // `.catch` function
});

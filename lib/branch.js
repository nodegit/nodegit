var NodeGit = require("../");
var Branch = NodeGit.Branch;

var _remoteName = Branch.remoteName;

/**
 * Retrieve the Branch's Remote Name as a String.
 *
 *  @async
 * @param {Repository} repo The repo to get the remote name from
 * @param {String} the refname of the branch
 * @return {String} remote name as a string.
 */
Branch.remoteName = function(repo, remoteRef) {
  return _remoteName.call(this, repo, remoteRef)
    .then(function(remoteNameBuffer) {
      return remoteNameBuffer.toString();
    });
};

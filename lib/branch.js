var NodeGit = require("../");
var Branch = NodeGit.Branch;

var _remoteName = Branch.remoteName;

/**
 * Retrieve the Branch's Remote Name as a String.
 *
 * @param {Repository} repo  The repo to get the remote name from.
 * @param {string}     the   Refname of the branch.
 * @returns {string} Remote. Name as a string.
 * @async
 */
Branch.remoteName = function (repo, remoteRef) {
  return _remoteName.call(this, repo, remoteRef).then(function (remoteNameBuffer) {
    return remoteNameBuffer.toString();
  });
};

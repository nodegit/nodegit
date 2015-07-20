var NodeGit = require("../");
var Promise = require("nodegit-promise");
var ConvenientLine = NodeGit.ConvenientLine;

function ConvenientHunk(hunk, linesInHunk, patch, i) {
  this.hunk = hunk;
  this.linesInHunk = linesInHunk;
  this.patch = patch;
  this.i = i;
}

/**
 * Diff header string that represents the context of this hunk
 * of the diff. Something like `@@ -169,14 +167,12 @@ ...`
 * @return {String}
 */
ConvenientHunk.prototype.header = function() {
  return this.hunk.header();
};

/**
 * Number of lines in this hunk
 * @return {Number}
 */
ConvenientHunk.prototype.size = function() {
  return this.linesInHunk;
};

/**
 * The lines in this hunk
 * @async
 * @return {Array<ConvenientLine>}  a promise that resolves to an array of
 *                                      ConvenientLines
 */
ConvenientHunk.prototype.lines = function() {
  var _this = this;
  var size = _this.size();
  var linePromises = [];
  var i;

  function makeLinePromise(i) {
    return _this.patch.getLineInHunk(_this.i, i)
      .then(function(line) {
        return new ConvenientLine(line, i);
      });
  }

  for (i = 0; i < size; i++) {
    linePromises.push(makeLinePromise(i));
  }

  return Promise.all(linePromises);
};

NodeGit.ConvenientHunk = ConvenientHunk;

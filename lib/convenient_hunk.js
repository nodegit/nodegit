function ConvenientHunk(raw, i) {
  this.raw = raw;
  this.i = i;
}

/**
 * Diff header string that represents the context of this hunk
 * of the diff. Something like `@@ -169,14 +167,12 @@ ...`
 * @return {String}
 */
ConvenientHunk.prototype.header = function() {
  return this.raw.hunk(this.i).header;
};

/**
 * Number of lines in this hunk
 * @return {Number}
 */
ConvenientHunk.prototype.size = function() {
  return this.raw.hunk(this.i).lines;
};

/**
 * The lines in this hunk
 * @return {[String]} array of strings
 */
ConvenientHunk.prototype.lines = function() {
  var result = [];
  for (var i = 0; i < this.size(); i++)
    result.push(this.raw.line(this.i, i));
  return result;
};

exports.ConvenientHunk = ConvenientHunk;

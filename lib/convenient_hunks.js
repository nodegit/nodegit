var NodeGit = require("../");

var ConvenientHunk = NodeGit.ConvenientHunk;

var header = ConvenientHunk.prototype.header;
 /**
  * Diff header string that represents the context of this hunk
  * of the diff. Something like `@@ -169,14 +167,12 @@ ...`
  * @return {String}
  */
ConvenientHunk.prototype.header = header;

var headerLen = ConvenientHunk.prototype.headerLen;
/**
 * The length of the header
 * @return {Number}
 */
ConvenientHunk.prototype.headerLen = headerLen;

var lines = ConvenientHunk.prototype.lines;
/**
 * The lines in this hunk
 * @async
 * @return {Array<DiffLine>}
 */
ConvenientHunk.prototype.lines = lines;

var newLines = ConvenientHunk.prototype.newLines;
/**
 * The number of new lines in the hunk
 * @return {Number}
 */
ConvenientHunk.prototype.newLines = newLines;

var newStart = ConvenientHunk.prototype.newStart;
/**
 * The starting offset of the first new line in the file
 * @return {Number}
 */
ConvenientHunk.prototype.newStart = newStart;

var oldLines = ConvenientHunk.prototype.oldLines;
/**
 * The number of old lines in the hunk
 * @return {Number}
 */
ConvenientHunk.prototype.oldLines = oldLines;

var oldStart = ConvenientHunk.prototype.oldStart;
/**
 * The starting offset of the first old line in the file
 * @return {Number}
 */
ConvenientHunk.prototype.oldStart = oldStart;

var size = ConvenientHunk.prototype.size;
/**
 * Number of lines in this hunk
 * @return {Number}
 */
ConvenientHunk.prototype.size = size;

var NodeGit = require("../");

var ConvenientHunk = NodeGit.ConvenientHunk;

var header = ConvenientHunk.prototype.header;
/**
 * Diff header string that represents the context of this hunk of the diff. Something like `@@ -169,14 +167,12 @@ ...`
 *
 * @returns {string}
 */
ConvenientHunk.prototype.header = header;

var headerLen = ConvenientHunk.prototype.headerLen;
/**
 * The length of the header.
 *
 * @returns {number}
 */
ConvenientHunk.prototype.headerLen = headerLen;

var lines = ConvenientHunk.prototype.lines;
/**
 * The lines in this hunk.
 *
 * @returns {DiffLine[]}
 * @async
 */
ConvenientHunk.prototype.lines = lines;

var newLines = ConvenientHunk.prototype.newLines;
/**
 * The number of new lines in the hunk.
 *
 * @returns {number}
 */
ConvenientHunk.prototype.newLines = newLines;

var newStart = ConvenientHunk.prototype.newStart;
/**
 * The starting offset of the first new line in the file.
 *
 * @returns {number}
 */
ConvenientHunk.prototype.newStart = newStart;

var oldLines = ConvenientHunk.prototype.oldLines;
/**
 * The number of old lines in the hunk.
 *
 * @returns {number}
 */
ConvenientHunk.prototype.oldLines = oldLines;

var oldStart = ConvenientHunk.prototype.oldStart;
/**
 * The starting offset of the first old line in the file.
 *
 * @returns {number}
 */
ConvenientHunk.prototype.oldStart = oldStart;

var size = ConvenientHunk.prototype.size;
/**
 * Number of lines in this hunk.
 *
 * @returns {number}
 */
ConvenientHunk.prototype.size = size;

var NodeGit = require("../");
var Diff = NodeGit.Diff;
var Patch = NodeGit.Patch;

var _blobToBuffer = Diff.blobToBuffer;

/**
 * Directly run a diff between a blob and a buffer.
 * @async
 * @param {Blob} old_blob Blob for old side of diff, or NULL for empty blob
 * @param {String} old_as_path Treat old blob as if it had this filename;
 * can be NULL
 * @param {String} buffer Raw data for new side of diff, or NULL for empty
 * @param {String} buffer_as_path Treat buffer as if it had this filename;
 * can be NULL
 * @param {DiffOptions} opts Options for diff, or NULL for default options
 * @param {Function} file_cb Callback for "file"; made once if there is a diff;
 * can be NULL
 * @param {Function} binary_cb Callback for binary files; can be NULL
 * @param {Function} hunk_cb Callback for each hunk in diff; can be NULL
 * @param {Function} line_cb Callback for each line in diff; can be NULL
 */
Diff.blobToBuffer= function(
    old_blob,
    old_as_path,
    buffer,
    buffer_as_path,
    opts,
    file_cb,
    binary_cb,
    hunk_cb,
    line_cb) {
  var bufferText;
  var bufferLength;
  if (buffer instanceof Buffer) {
    bufferText = buffer.toString("utf8");
    bufferLength = Buffer.byteLength(buffer, "utf8");
  } else {
    bufferText = buffer;
    bufferLength = !buffer ? 0 : Buffer.byteLength(buffer, "utf8");
  }

  return _blobToBuffer.call(
    this,
    old_blob,
    old_as_path,
    bufferText,
    bufferLength,
    buffer_as_path,
    opts,
    file_cb,
    binary_cb,
    hunk_cb,
    line_cb,
    null);
};

/**
 * Retrieve patches in this difflist
 *
 * @async
 * @return {Array<ConvenientPatch>} a promise that resolves to an array of
 *                                      ConvenientPatches
 */
Diff.prototype.patches = function(idxs) {
  return Patch.convenientFromDiff(this, idxs);
};

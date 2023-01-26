var NodeGit = require("../");

var DiffFile = NodeGit.DiffFile;

var flags = DiffFile.prototype.flags;
/**
 * Returns the file's flags.
 *
 * @returns {number}
 */
DiffFile.prototype.flags = flags;

var id = DiffFile.prototype.id;
/**
 * Returns the file's Oid.
 *
 * @returns {Oid}
 */
DiffFile.prototype.id = id;

var mode = DiffFile.prototype.mode;
/**
 * Returns the file's mode.
 *
 * @returns {number}
 */
DiffFile.prototype.mode = mode;

var path = DiffFile.prototype.path;
/**
 * Returns the file's path.
 *
 * @returns {string}
 */
DiffFile.prototype.path = path;

var size = DiffFile.prototype.size;
/**
 * Returns the file's size.
 *
 * @returns {number}
 */
DiffFile.prototype.size = size;

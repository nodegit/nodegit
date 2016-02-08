var NodeGit = require("../");

var DiffFile = NodeGit.DiffFile;

var flags = DiffFile.prototype.flags;
/**
 * Returns the file's flags
 * @return {Number}
 */
DiffFile.prototype.flags = flags;

var id = DiffFile.prototype.id;
/**
 * Returns the file's Oid
 * @return {Oid}
 */
DiffFile.prototype.id = id;

var mode = DiffFile.prototype.mode;
/**
 * Returns the file's mode
 * @return {Number}
 */
DiffFile.prototype.mode = mode;

var path = DiffFile.prototype.path;
/**
 * Returns the file's path
 * @return {String}
 */
DiffFile.prototype.path = path;

var size = DiffFile.prototype.size;
/**
 * Returns the file's size
 * @return {Number}
 */
DiffFile.prototype.size = size;

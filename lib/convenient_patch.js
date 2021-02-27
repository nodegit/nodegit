var NodeGit = require("../");

var ConvenientPatch = NodeGit.ConvenientPatch;

var hunks = ConvenientPatch.prototype.hunks;
/**
 * The hunks in this patch.
 *
 * @returns {Promise<ConvenientHunk[]>}
 * @async
 */
ConvenientPatch.prototype.hunks = hunks;

var isAdded = ConvenientPatch.prototype.isAdded;
/**
 * Is this an added patch?
 *
 * @returns {boolean}
 */
ConvenientPatch.prototype.isAdded = isAdded;

var isConflicted = ConvenientPatch.prototype.isConflicted;
/**
 * Is this a conflicted patch?
 *
 * @returns {boolean}
 */
ConvenientPatch.prototype.isConflicted = isConflicted;

var isCopied = ConvenientPatch.prototype.isCopied;
/**
 * Is this a copied patch?
 *
 * @returns {boolean}
 */
ConvenientPatch.prototype.isCopied = isCopied;

var isDeleted = ConvenientPatch.prototype.isDeleted;
/**
 * Is this a deleted patch?
 *
 * @returns {boolean}
 */
ConvenientPatch.prototype.isDeleted = isDeleted;

var isIgnored = ConvenientPatch.prototype.isIgnored;
/**
 * Is this an ignored patch?
 *
 * @returns {boolean}
 */
ConvenientPatch.prototype.isIgnored = isIgnored;

var isModified = ConvenientPatch.prototype.isModified;
/**
 * Is this an modified patch?
 *
 * @returns {boolean}
 */
ConvenientPatch.prototype.isModified = isModified;

var isRenamed = ConvenientPatch.prototype.isRenamed;
/**
 * Is this a renamed patch?
 *
 * @returns {boolean}
 */
ConvenientPatch.prototype.isRenamed = isRenamed;

var isTypeChange = ConvenientPatch.prototype.isTypeChange;
/**
 * Is this a type change?
 *
 * @returns {boolean}
 */
ConvenientPatch.prototype.isTypeChange = isTypeChange;

var isUnmodified = ConvenientPatch.prototype.isUnmodified;
/**
 * Is this an unmodified patch?
 *
 * @returns {boolean}
 */
ConvenientPatch.prototype.isUnmodified = isUnmodified;

var isUnreadable = ConvenientPatch.prototype.isUnreadable;
/**
 * Is this an undreadable patch?
 *
 * @returns {boolean}
 */
ConvenientPatch.prototype.isUnreadable = isUnreadable;

var isUntracked = ConvenientPatch.prototype.isUntracked;
/**
 * Is this an untracked patch?
 *
 * @returns {boolean}
 */
ConvenientPatch.prototype.isUntracked = isUntracked;

/**
 * @type {Object}
 * @typedef lineStats
 * @property {number} total_context    # of contexts in the patch.
 * @property {number} total_additions  # of lines added in the patch.
 * @property {number} total_deletions  # of lines deleted in the patch.
 */
var lineStats = ConvenientPatch.prototype.lineStats;
/**
 * The line statistics of this patch (#contexts, #added, #deleted)
 *
 * @returns {lineStats}
 */
ConvenientPatch.prototype.lineStats = lineStats;

var newFile = ConvenientPatch.prototype.newFile;
/**
 * New attributes of the file.
 *
 * @returns {DiffFile}
 */
ConvenientPatch.prototype.newFile = newFile;

var oldFile = ConvenientPatch.prototype.oldFile;
/**
 * Old attributes of the file.
 *
 * @returns {DiffFile}
 */
ConvenientPatch.prototype.oldFile = oldFile;

var size = ConvenientPatch.prototype.size;
/**
 * The number of hunks in this patch.
 *
 * @returns {number}
 */
ConvenientPatch.prototype.size = size;

var status = ConvenientPatch.prototype.status;
/**
 * The status of this patch (unmodified, added, deleted)
 *
 * @returns {number}
 */
ConvenientPatch.prototype.status = status;

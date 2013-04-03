// Used to detect for Cygwin
var os = require('os');

// Required for Windows/Cygwin support
var root = [__dirname, '/../vendor/libgit2/build/shared'].join(''),
  path = process.env.PATH;

if (~os.type().indexOf('CYGWIN') && !~path.indexOf(root)) {
  process.env.PATH = root + ':' + path;
}

// Import libraries
exports.blob = require('./blob.js').blob;
exports.repo = require('./repo.js').repo;
exports.signature = require('./signature.js').signature;
exports.oid = require('./oid.js').oid;
exports.reference = require('./reference.js').reference;
exports.revwalk = require('./revwalk.js').revwalk;
exports.commit = require('./commit.js').commit;
exports.tree = require('./tree.js').tree;

// Assign raw api to module
try {
  exports.raw = require('../build/Debug/nodegit');
} catch (error) {
  exports.raw = require('../build/Release/nodegit');
}

// Initialize objects that need to access their raw counterparts
exports.diffList = require('./diff_list.js').diffList;
exports.error = require('./error.js').error;
exports.entry = require('./tree_entry.js').entry;

// Set version
exports.version = require('../package').version;

// Initialize threads
(new exports.raw.Threads()).init();

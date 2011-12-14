// Used to detect for Cygwin
var os = require("os");

// Required for Windows/Cygwin support
var root = [ __dirname, "/../vendor/libgit2/build/shared" ].join(""),
  path = process.env.PATH;

if (~os.type().indexOf("CYGWIN") && !~path.indexOf(root)) {
  process.env.PATH = root + ":" + path;
}

// Import libraries
exports.util = require("./util.js").util;
exports.blob = require("./blob.js").blob;
exports.repo = require("./repo.js").repo;
exports.error = require("./error.js").error;
exports.sig = require("./sig.js").sig;
exports.oid = require("./oid.js").oid;
exports.object = require("./object.js").object;
exports.ref = require("./ref.js").ref;
exports.revwalk = require("./revwalk.js").revwalk;
exports.commit = require("./commit.js").commit;
exports.tree = require("./tree.js").tree;
exports.entry = require("./tree_entry.js").entry;

// Assign raw api to module
exports.raw = require("../build/Release/nodegit");
// Set version
exports.version = "0.0.6";

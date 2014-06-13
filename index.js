var rawApi;

// Attempt to load the production release first, if it fails fall back to the
// debug release.
try {
  rawApi = require('./build/Release/nodegit');
}
catch (e) {
  rawApi = require('./build/Debug/nodegit');
}

// Set the exports prototype to the raw API.
exports.__proto__ = rawApi;

// Import extensions
require('./lib/commit.js');
require('./lib/blob.js');
require('./lib/object.js');
require('./lib/signature.js');
require('./lib/odb.js');
require('./lib/oid.js');
require('./lib/index.js');
require('./lib/repo.js');
require('./lib/reference.js');
require('./lib/revwalk.js');
require('./lib/tree.js');
require('./lib/diff_list.js');
require('./lib/tree_entry.js');
require('./lib/tree_builder.js');

// Set version
exports.version = require('./package').version;

// Initialize threads
exports.Threads.init();

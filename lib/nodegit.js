var rawApi;

// Attempt to load the production release first, if it fails fall back to the
// debug release.
try {
  rawApi = require('../build/Release/nodegit');
}
catch (e) {
  rawApi = require('../build/Debug/nodegit');
}

// Set the exports prototype to the raw API.
exports.__proto__ = rawApi;

// Import extensions
require('./commit.js');
require('./blob.js');
require('./object.js');
require('./signature.js');
require('./odb.js');
require('./oid.js');
require('./index.js');
require('./repository.js');
require('./reference.js');
require('./revwalk.js');
require('./tree.js');

// Set version
exports.version = require('../package').version;

// Initialize threads
exports.Threads.threadsInit();

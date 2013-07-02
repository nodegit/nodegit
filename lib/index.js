// Used to detect for Cygwin
var os = require('os');

// Required for Windows/Cygwin support
var root = [__dirname, '/../vendor/libgit2/build/shared'].join(''),
  path = process.env.PATH;

if (~os.type().indexOf('CYGWIN') && !~path.indexOf(root)) {
  process.env.PATH = root + ':' + path;
}

// Assign raw api to module
var rawApi = require('../build/Release/nodegit');
for (var key in rawApi) {
  exports[key] = rawApi[key];
}

// Import extensions
require('./commit.js');
require('./blob.js');
require('./repo.js');
require('./reference.js');
require('./revwalk.js');
require('./tree.js');
require('./diff_list.js');
require('./tree_entry.js');

// Set version
exports.version = require('../package').version;

// Initialize threads
exports.Threads.init();

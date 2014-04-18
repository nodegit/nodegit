var promisify = require('promisify-node');
var descriptors = require('../generate/v0.20.0.json');
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
require('./diff.js');
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

// Wrap asynchronous methods to return promises.
promisify(exports);

// Native methods do not return an identifiable function, so this function will
// filter down the function identity to match the libgit2 descriptor.
promisify(rawApi, function(func, keyName, parentKeyName) {
  // Find the correct descriptor.
  var descriptor = descriptors.filter(function(descriptor) {
    var key = parentKeyName ? parentKeyName.slice(0, -1) : keyName; 
    return descriptor.jsClassName === key;
  })[0];

  // If this is a top level construct, recurse into.
  if (!parentKeyName) {
    return true;
  }

  // Determine if this is a prototype method.
  var isPrototypeMethod = parentKeyName.slice(-1) === "#";

  if (descriptor && descriptor.functions) {
    // Find the nested function in the descriptor.
    if (parentKeyName.indexOf("Repo") > -1) { console.log(arguments); }
    var nestedFunction = descriptor.functions.filter(function(nestedFunction) {
      var test = keyName || func.name || parentKeyName;

      if (nestedFunction.jsFunctionName === keyName) {
        return func.isPrototypeMethod === isPrototypeMethod;
      }
    })[0];

    // Verify it is an asynchronous method.
    //console.log(nestedFunction && nestedFunction.isAsync);
    return nestedFunction && nestedFunction.isAsync;
  }
});

// Set version.
exports.version = require('./package').version;

// Initialize threads.
exports.Threads.init();

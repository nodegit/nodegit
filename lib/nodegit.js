var promisify = require("promisify-node");
var descriptors = require("../generate/idefs.json");
var rawApi;

// Attempt to load the production release first, if it fails fall back to the
// debug release.
try {
  rawApi = require("../build/Release/nodegit");
}
catch (e) {
  /* istanbul ignore next */
  rawApi = require("../build/Debug/nodegit");
}

// Native methods do not return an identifiable function, so this function will
// filter down the function identity to match the libgit2 descriptor.
descriptors.forEach(function(descriptor) {
  var Ctor =  rawApi[descriptor.jsClassName];

  // Iterate over each function in the file.
  descriptor.functions.filter(function(func) {
    return func.isAsync;
  }).forEach(function(asyncFunc) {
    var original = null;

    // Special case when you have a prototype method.
    if (asyncFunc.isPrototypeMethod) {
      original = Ctor.prototype[asyncFunc.jsFunctionName];
      Ctor.prototype[asyncFunc.jsFunctionName] = promisify(original);
    }
    else {
      original = Ctor[asyncFunc.jsFunctionName];
      Ctor[asyncFunc.jsFunctionName] = promisify(original);
    }
  });
});

// Set the exports prototype to the raw API.
exports.__proto__ = rawApi;

// Import extensions
require("./commit.js");
require("./diff.js");
require("./blob.js");
require("./object.js");
require("./signature.js");
require("./odb.js");
require("./oid.js");
require("./index.js");
require("./repository.js");
require("./refs.js");
require("./revwalk.js");
require("./tree.js");

// Wrap asynchronous methods to return promises.
promisify(exports);

// Set version.
exports.version = require("../package").version;

// Initialize threads.
exports.Threads.threadsInit();

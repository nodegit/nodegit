// Assign raw api to module
var rawApi = require("../build/emscripten/nodegit");

// Set the exports prototype to the raw API.
exports.__proto__ = rawApi;

// Set version
exports.version = require("../package").version;

// Initialize threads
//exports.Threads.init();

var glob = typeof window !== "undefined" ? window : global;

glob.nodegit = exports;
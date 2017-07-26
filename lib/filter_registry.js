var NodeGit = require("../");
var normalizeOptions = NodeGit.Utils.normalizeOptions;

var FilterRegistry = NodeGit.FilterRegistry;

var _register = FilterRegistry.register;
var _unregister = FilterRegistry.unregister;

// register should add filter by name to dict and return
// Override FilterRegistry.register to normalize Filter
FilterRegistry.register = function(name, filter, priority, callback) {
  // setting default value of attributes
  if (filter.attributes === undefined) {
    filter.attributes = "";
  }

  filter = normalizeOptions(filter, NodeGit.Filter);

  if (!filter.check || !filter.apply) {
    return callback(new Error(
      "ERROR: please provide check and apply callbacks for filter"
    ));
  }

  return _register(name, filter, priority)
    .then(function(result) {
      if (typeof callback === "function") {
        callback(null, result);
      }
      return result;
    }, callback);
};

FilterRegistry.unregister = function(name, callback) {
  return _unregister(name)
    .then(function(result) {
      if (typeof callback === "function") {
          callback(null, result);
      }
      return result;
    }, callback);
};

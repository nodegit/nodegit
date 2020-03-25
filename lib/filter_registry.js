var NodeGit = require("../");
var normalizeOptions = NodeGit.Utils.normalizeOptions;

var FilterRegistry = NodeGit.FilterRegistry;

var _register = FilterRegistry.register;

// register should add filter by name to dict and return
// Override FilterRegistry.register to normalize Filter
FilterRegistry.register = function(name, filter, priority) {
  // setting default value of attributes
  if (filter.attributes === undefined) {
    filter.attributes = "";
  }

  filter = normalizeOptions(filter, NodeGit.Filter);

  if (!filter.check || !filter.apply) {
    return Promise.reject(new Error(
      "ERROR: please provide check and apply callbacks for filter"
    ));
  }

  return _register(name, filter, priority);
};

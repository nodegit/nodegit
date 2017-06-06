var NodeGit = require("../");
var normalizeOptions = NodeGit.Utils.normalizeOptions;

var FilterRegistry = NodeGit.FilterRegistry;

var _register = FilterRegistry.register;

// Override FilterRegistry.register to normalize Filter
FilterRegistry.register = function(name, filter, priority) {
  filter = normalizeOptions(filter, NodeGit.Filter);
  console.log("[DEBUG] Filter: ", filter);
  if(filter.check && filter.apply) {
    return _register(name, filter, priority);
  }
  else {
    console.log(
      "ERROR: please provide check and (apply or stream) callbacks for filter");
    return null;
  }
};

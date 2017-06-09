var NodeGit = require("../");
var normalizeOptions = NodeGit.Utils.normalizeOptions;

var FilterRegistry = NodeGit.FilterRegistry;

var _register = FilterRegistry.register;
var _unregister = FilterRegistry.unregister;

// // hold onto the scope of our filters until unregister is called
var filtersByName = {};

// register should add filter by name to dict and return

// Override FilterRegistry.register to normalize Filter
FilterRegistry.register = function(name, filter, priority) {
  // setting default value of attributes
  if(filter.attributes === undefined) {
    filter.attributes = "";
  }

  filter = normalizeOptions(filter, NodeGit.Filter);

  if (filtersByName[name] === undefined) {
    filtersByName[name] = filter;
  }

  console.log("filtersByName[Register]: ", filtersByName);


  if(filter.check && filter.apply) {
    return _register(name, filter, priority);
  }
  else {
    console.log(
      "ERROR: please provide check and apply callbacks for filter");
    return null;
  }
};

FilterRegistry.unregister = function(name){
  if (filtersByName[name] !== undefined) {
    delete filtersByName[name];
  }
  
  console.log("filtersByName[Unregister]: ", filtersByName);

  return _unregister(name);
};

module.exports = function(functions) {
  if (!functions || functions.length === 0) {
    throw new Error("Should not be able to get function from empty function list");
  }

  const fun = functions.find(function(f) { return f.useAsOnRootProto; });
  if (!fun) {
    throw new Error("There is no function on the root prototype for this collection");
  }

  return fun.cppFunctionName;
};

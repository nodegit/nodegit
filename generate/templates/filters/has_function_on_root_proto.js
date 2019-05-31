module.exports = function(functions) {
  if (!functions || functions.length === 0) {
    return false;
  }

  return functions.some(function(f) { return f.useAsOnRootProto; });
};

function shallowClone() {
  var merges = Array.prototype.slice.call(arguments);

  return merges.reduce(function(obj, merge) {
    return Object.keys(merge).reduce(function(obj, key) {
      obj[key] = merge[key];
      return obj;
    }, obj);
  }, {});
}

module.exports = shallowClone;

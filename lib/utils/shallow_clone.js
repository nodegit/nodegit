var NodeGit = require("../../");

function shallowClone() {
  var merges = Array.prototype.slice.call(arguments);

  return merges.reduce(function(obj, merge) {
    return Object.keys(merge).reduce(function(obj, key) {
      obj[key] = merge[key];
      return obj;
    }, obj);
  }, {});
}

NodeGit.Utils.shallowClone = module.exports = shallowClone;

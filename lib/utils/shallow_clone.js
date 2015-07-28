var NodeGit = require("../../");

function shallowClone(obj) {
  var merges = Array.prototype.slice.call(arguments, 1);

  return merges.reduce(function(obj, merge) {
    return Object.keys(merge).reduce(function(obj, key) {
      obj[key] = merge[key];
      return obj;
    }, obj);
  }, obj || {});
}

NodeGit.Utils.shallowClone = module.exports = shallowClone;

var NodeGit = require("../");
var CloneOptions = NodeGit.CloneOptions;

/**
 * Turn a JavaScript object into a CloneObject
 *
 * @param {Object} obj - The JS object to be converted
 * @return {Object} A CloneObject instance.
 */
CloneOptions.fromObj = function(obj) {
  try {
    if (typeof obj != "object") return null;

    var result = new CloneOptions();

    Object.keys(obj).forEach(function(property) {
      result[property] = obj[property];
    });

    return result;
  }
  catch (ex) {
    return null;
  }
}

module.exports = CloneOptions;

var Promise = require("nodegit-promise");

/**
* Wraps a method so that you can pass in either a string, OID or the object
* itself and you will always get back a promise that resolves to the object.
* @param {Object} objectType The object type that you're expecting to receive.
* @param {Function} lookupFunction  The function to do the lookup for the
*                                   object. Defaults to `objectType.lookup`.
* @return {Function}
*/
module.exports = function(objectType, lookupFunction) {
  lookupFunction = lookupFunction || objectType.lookup;
  
  return function(repo, id, callback) {
    if (id instanceof objectType) {
      return Promise.resolve(id).then(function(obj) {
        obj.repo = repo;

        if (typeof callback === "function") {
          callback(null, obj);
        }

        return obj;
      }, callback);
    }

    return lookupFunction(repo, id).then(function(obj) {
      obj.repo = repo;

      if (typeof callback === "function") {
        callback(null, obj);
      }

      return obj;
    }, callback);
  };
};

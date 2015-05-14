var NodeGit = require("../../");

/**
 * Normalize an object to match a struct.
 *
 * @param {String, Object} oid - The oid string or instance.
 * @return {Object} An Oid instance.
 */
function normalizeOptions(options, Ctor) {
  if (!options) {
    return null;
  }

  if (options instanceof Ctor) {
    return options;
  }

  var instance = new Ctor();

  Object.keys(options).forEach(function(key) {
    instance[key] = options[key];
  });

  return instance;
}

NodeGit.Utils.normalizeOptions = normalizeOptions;

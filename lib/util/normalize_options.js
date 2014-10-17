/**
 * Normalize an object to match a struct.
 *
 * @param {String, Object} oid - The oid string or instance.
 * @return {Object} An Oid instance.
 */
function normalizeOptions(options, Ctor) {
  var instance = options instanceof Ctor ? options : new Ctor();

  if (!options) {
    return null;
  }

  Object.keys(options).forEach(function(key) {
    instance[key] = options[key];
  });

  return instance;
}

module.exports = normalizeOptions;

var git = require('../');

exports.makeSafe = function(object, key) {
  var oldFn = object[key];
  object[key] = function() {
    try {
      return oldFn.apply(this, arguments);
    } catch (e) {
      var callback = arguments[arguments.length - 1];
      callback(e);
    }
  };
};

exports.normalizeOid = function(object, key) {
  var oldFn = object[key];
  object[key] = function(oid) {
    if (typeof oid === 'string') oid = git.Oid.fromString(oid);
    return oldFn.apply(this, arguments);
  };
};

var git = require('../');

exports.makeSafe = function(object, key) {
  var oldFn = object[key];
  object[key] = function() {
    try {
      oldFn.apply(this, arguments);
    } catch (e) {
      var callback = arguments[arguments.length - 1];
      callback(e);
    }
  };
};

exports.normalizeOid = function(object, key) {
  var oldFn = object[key];
  object[key] = function() {
    var oid = arguments[0];
    if (typeof oid === 'string') oid = git.Oid.fromString(oid);
    arguments[0] = oid;
    oldFn.apply(this, arguments);
  };
};

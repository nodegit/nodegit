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

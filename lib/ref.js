var git = require('../');

var _Ref = function(obj) {
  var self = {};

  if(obj instanceof git.raw.Repo) {
    self.repo = obj;
    self.ref = new git.raw.Ref(obj);
  } else if(obj instanceof git.raw.Ref) {
    self.ref = obj;
  }

  self.lookup = function(name, callback) {
    if(!callback || typeof callback !== 'function') {
      throw new Error('Callback is required and must be a function');
    }

    self.ref.lookup(self.repo, name, function(error) {
      if (error) {
        callback(git.error(error), self);
        return;
      }
      callback(null, self);
    });
  };

  self.oid = function() {
    var oid = git.oid();
    self.ref.oid(oid.oid);
    return oid;
  };

  return self;
};

exports.ref = _Ref;

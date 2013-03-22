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
    self.ref.lookup(self.repo, name, function(error) {
      console.log('1', arguments);
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

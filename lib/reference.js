var git = require('../');

var Reference = function(rawReference) {
  var self = {};

  if(rawReference instanceof git.raw.Reference) {
    self.reference = rawReference;
  } else if(rawReference instanceof git.raw.Repo) {
    self.repo = rawReference;
    self.reference = new git.raw.Reference(rawReference);
  }

  self.lookup = function(name, callback) {
    self.reference.lookup(self.repo, name, function(error, reference) {
      if (error) {
        callback(git.error(error), self);
        return;
      }
      self.reference = reference;
      callback(null, self);
    });
  };

  self.oid = function(callback) {
    self.reference.oid(function(error, rawOid) {
      if (error) {
        callback(git.error(error, self));
        return;
      }
      callback(null, git.oid(rawOid));
    });
  };

  return self;
};

exports.reference = Reference;

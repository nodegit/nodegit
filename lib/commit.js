var FFI = require('node-ffi');

var GITSignature = require('./structs').GITSignature

var Oid = require('./oid').Oid;

var libgit2 = new FFI.Library('libgit2', {
  //'git_commit_lookup'         : ['int32', ['pointer', 'pointer', 'pointer']],
  //'git_commit_new'            : ['int32', ['pointer', 'pointer']],
  'git_commit_id'             : ['pointer', ['pointer']],
  'git_commit_message_short'  : ['string', ['pointer']],
  'git_commit_message'        : ['string', ['pointer']],
  //'git_commit_time'           : ['time_t', ['pointer']],
  //'git_commit_timezone_offset': ['int32', ['pointer']],
  'git_commit_committer'      : ['pointer', ['pointer']],
  'git_commit_author'         : ['pointer', ['pointer']],
  'git_commit_tree'           : ['pointer', ['pointer']],
  'git_commit_parentcount'    : ['uint32', ['pointer']],
  'git_commit_parent'         : ['pointer', ['pointer', 'uint32']],
  'git_commit_add_parent'     : ['int32', ['pointer', 'pointer']],
  'git_commit_set_message'    : ['void', ['pointer', 'string']],
  'git_commit_set_committer'  : ['void', ['pointer', 'pointer']],
  'git_commit_set_author'     : ['void', ['pointer', 'pointer']],
  'git_commit_set_tree'       : ['void', ['pointer', 'pointer']],
});

var Commit = exports.Commit = function() {
  var _commit = null;
  if(arguments[0] instanceof FFI.Pointer) {
    _commit = arguments[0];
  } else {
    _commit = new FFI.Pointer(FFI.Bindings.POINTER_SIZE);
  }

  Object.defineProperty(this, "ccommit", { get: function() { return _commit; }, enumerable: true});
  Object.defineProperty(this, "id",
    { get: function() { return new Oid(libgit2.git_commit_id(_commit)); },
    enumeragle: true });
  Object.defineProperty(this, "message",
    {
      get: function() { return libgit2.git_commit_message(_commit); },
      enumerable: true,
    });
  Object.defineProperty(this, "message_short",
    {
      get: function() { return libgit2.git_commit_message_short(_commit); },
      enumerable: true,
    });
  Object.defineProperty(this, "author",
    {
      get: function() { return new GITSignature(libgit2.git_commit_author(_commit)); },
      enumerable: true,
    });
  Object.defineProperty(this, "committer",
    {
      get: function() { return new GITSignature(libgit2.git_commit_committer(_commit)); },
      enumerable: true,
    });
}

var FFI = require('node-ffi');
var libgit2 = new FFI.Library('libgit2', {
  'git_repository_open'     : ['int32', ['pointer', 'string']],
  'git_repository_lookup'   : ['int32', ['pointer', 'pointer', 'pointer', 'int32']],
  'git_repository_database' : ['pointer', ['pointer']],
  'git_repository_index'    : ['pointer', ['pointer']],
  'git_repository_newobject': ['int32', ['pointer', 'pointer', 'int32']],
  'git_repository_free'     : ['void', ['pointer']],
  'git_repository_init'         : ['int32', ['pointer', 'string', 'uchar']],
  'git_repository_lookup_ref'   : ['int32', ['pointer', 'pointer', 'string']],
});
var fs = require('fs');
var path = require('path');

var Oid = require('./oid').Oid;
var Commit = require('./commit').Commit;
var RevWalk = require('./revwalk').RevWalk;
var Ref = require('./refs').Ref;

var git_raise_error = require('./error').git_raise_error;

var Repo = exports.Repo = function(repo_path) {
  var _repo_path = repo_path
  var _repo_tmp = new FFI.Pointer(FFI.Bindings.POINTER_SIZE);

  var ret = libgit2.git_repository_open(_repo_tmp, _repo_path)

  git_raise_error(ret);

  var _repo = _repo_tmp.getPointer();

  Object.defineProperty(this, "crepo", { get: function() { return _repo; }, enumerable: true});
  Object.defineProperty(this, "path", { get: function() { return _repo_path; }, enumerable: true});;
}

Repo.prototype.close = function() {
  libgit2.git_repository_free(this.crepo);
}

Repo.prototype.refs = function(arg, callback) {
    var _ref = new FFI.Pointer(FFI.Bindings.POINTER_SIZE)
    var ret = libgit2.git_repository_lookup_ref(_ref, this.crepo, arg)
    git_raise_error(ret)
    callback(new Ref(_ref.getPointer()));
}

Repo.prototype.lookup = function(arg) {
  var oid = null;
  if(arg instanceof String || typeof arg == "string") {
    oid = new Oid(arg);
    var tmp = oid.toString()
    if(arg != tmp) {
      throw("OID Failed roundtrip: "+ arg +" != "+tmp)
    }
  } else if (arg instanceof Oid) {
    oid = arg;
  } else if (arg instanceof Ref) {
    oid = arg.oid;
  } else {
    console.log('not a string: ', typeof arg, arg)
  }
  
  var _commit = new FFI.Pointer(FFI.Bindings.POINTER_SIZE);
  var ret = libgit2.git_repository_lookup(_commit,
    this.crepo, oid.coid, Oid.GIT_OBJ_COMMIT);
  git_raise_error(ret);
  return new Commit(_commit.getPointer());
}

Repo.prototype.walk = function(head, callback) {
  var revwalk = new RevWalk(this)
  var head_commit = this.lookup(head)
  revwalk.sorting(RevWalk.SORT_TOPOLOGICAL)
  revwalk.push(head_commit)
  var commit = revwalk.next()
  while(commit) {
    callback(commit);
    commit = revwalk.next()
  }
  revwalk.close()
}

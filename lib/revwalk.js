var FFI = require('node-ffi');

var libgit2 = new FFI.Library('libgit2', {
  'git_revwalk_new'     : ['int32', ['pointer', 'pointer']],
  'git_revwalk_reset'   : ['void', ['pointer']],
  'git_revwalk_push'    : ['int32', ['pointer', 'pointer']],
  'git_revwalk_hide'    : ['int32', ['pointer', 'pointer']],
  'git_revwalk_next'    : ['pointer', ['pointer']],
  'git_revwalk_sorting' : ['int32', ['pointer', 'uint32']],
  'git_revwalk_free'    : ['pointer', ['pointer']],
});

var Commit = require('./commit').Commit

var git_raise_error = require('./error').git_raise_error

var RevWalk = exports.RevWalk = function(repo) {
  var _revwalk = new FFI.Pointer(FFI.Bindings.POINTER_SIZE);
  var ret = libgit2.git_revwalk_new(_revwalk, repo.crepo);

  git_raise_error(ret)

  _revwalk = _revwalk.getPointer();

  Object.defineProperty(this, "crevwalk", { get: function() { return _revwalk; }, enumerable: true});
}

RevWalk.prototype.reset = function () {
  libgit2.git_revwalk_reset(this.crevwalk);
}

RevWalk.prototype.push = function(commit) {
  return libgit2.git_revwalk_push(this.crevwalk, commit.ccommit);
}

RevWalk.prototype.hide = function(commit) {
  return libgit2.git_revwalk_hide(this.crevwalk, commit.ccommit);
}

RevWalk.prototype.next = function() {
  var ptr = libgit2.git_revwalk_next(this.crevwalk);
  if(ptr.isNull()) {
    return null;
  } else {
    return new Commit(ptr)
  }
}

RevWalk.prototype.sorting = function(flags) {
  return libgit2.git_revwalk_sorting(this.crevwalk, flags);
}

RevWalk.prototype.close = function() {
  return libgit2.git_revwalk_free(this.crevwalk);
}

RevWalk.SORT_NONE = 0
RevWalk.SORT_TOPOLOGICAL = (1 << 0)
RevWalk.SORT_TIME = (1 << 1)
RevWalk.SORT_REVERSE = (1 << 2)

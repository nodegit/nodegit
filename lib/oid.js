var FFI = require('node-ffi');

var git_raise_error = require('./error').git_raise_error;

var libgit2 = new FFI.Library('libgit2', {
  'git_oid_mkstr'     : ['int32', ['pointer', 'string']],
  'git_oid_mkraw'     : ['void', ['pointer', 'pointer']],
  'git_oid_fmt'       : ['void', ['pointer', 'pointer']],
  'git_oid_pathfmt'   : ['void', ['string', 'pointer']],
  'git_oid_allocfmt'  : ['string', ['pointer']],
  'git_oid_to_string' : ['string', ['pointer', 'size_t', 'pointer']],
  'git_oid_cpy'       : ['void', ['pointer', 'pointer']],
  'git_oid_cmp'       : ['int32', ['pointer', 'pointer']],
});

var oid_build = []
for(i=0; i < 40; ++i) {
  oid_build.push(['uchar', 'byte'+i]);
}

var OIDStruct = new FFI.Struct(oid_build);



var Oid = exports.Oid = function(arg) {
  var _oid;
  if (arg instanceof FFI.Pointer) {
    _oid = arg;
  } else if (arg instanceof String || arg instanceof Buffer || typeof arg =="string") {
    //_oid = new FFI.Pointer(FFI.Bindings.POINTER_SIZE);
    _oid = new FFI.Pointer(20);
    var ret = libgit2.git_oid_mkstr(_oid, arg)
    git_raise_error(ret);

  }
  Object.defineProperty(this, "coid",
    { get: function() { return _oid; }, enumerable: true});
}

Oid.prototype.toString = function() {
  //var _str = new FFI.Pointer(FFI.Bindings.POINTER_SIZE);
  var _str = new FFI.Pointer(41);
  libgit2.git_oid_fmt(_str, this.coid);
  var buff = new Buffer(40);
  for(i=0; i<40; ++i) {
    buff[i] = _str.seek(i).getUInt8();
  }
  delete _ostr
  return buff.toString('utf8')
}

Oid.GIT_OBJ_ANY = -2		  //Object can be any of the following
Oid.GIT_OBJ_BAD = -1      //Object is invalid.
Oid.GIT_OBJ__EXT = 0      //Reserved for future use.
Oid.GIT_OBJ_COMMIT = 1    //A commit object.
Oid.GIT_OBJ_TREE = 2      //A tree (directory listing) object.
Oid.GIT_OBJ_BLOB = 3      //A file revision object.
Oid.GIT_OBJ_TAG = 4       //An annotated tag object.
Oid.GIT_OBJ__EXT2 = 5     //Reserved for future use.
Oid.GIT_OBJ_OFS_DELTA = 6 //A delta, base is given by an offset.
Oid.GIT_OBJ_REF_DELTA = 7 //A delta, base is given by object id.

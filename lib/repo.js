var FFI = require('node-ffi');
var libgit2 = new FFI.Library('libgit2', {
  'git_repository_open'     : ['int32', ['pointer', 'string']],
  'git_repository_lookup'   : ['int32', ['pointer', 'pointer', 'pointer', 'int32']],
  'git_repository_database' : ['pointer', ['pointer']],
  'git_repository_index'    : ['pointer', ['pointer']],
  'git_repository_newobject': ['int32', ['pointer', 'pointer', 'int32']],
  'git_repository_free'     : ['void', ['pointer']],
  'git_repository_init'     : ['int32', ['pointer', 'string', 'uchar']],
});
var fs = require('fs');
var path = require('path');

var Oid = require('./oid').Oid;
var Commit = require('./commit').Commit;
var RevWalk = require('./revwalk').RevWalk;

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

Repo.prototype.refs = function(callback) {
  var refs = path.join(this.path, 'refs')
  var repo = this;
  //TODO FIXME XXX -- This is all Sync because I get weird double free's in ffi? gc problems?
  //TODO FIXME XXX -- Packed refs need read as well
  function handleFiles(section, files) {
    files.forEach(function(f) {
      var ref = path.join('refs', section, f);
      data = fs.readFileSync(path.join(repo.path, ref));
      callback(ref, data.toString('utf8').replace(/\s+$/, ''));
    });
  }
  var files = fs.readdirSync(path.join(refs, 'heads'));
  handleFiles('heads', files);
  /*fs.readdir(path.join(refs, 'remotes'), function(err, files){
    //handleFiles('remotes', files);
  })
  fs.readdir(path.join(refs, 'tags'), function(err, files){
    //handleFiles('tags', files);
  });*/
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
  } else {
    console.log('not a string: ' + typeof arg)
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
  revwalk.push(head_commit)
  var commit = revwalk.next()
  while(commit) {
    callback(commit);
    commit = revwalk.next()
  }
  revwalk.close()
}

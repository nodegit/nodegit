var FFI = require('node-ffi')
  , GITSignature = require('./structs').GITSignature
  , Oid = require('./oid').Oid
  , type = {

      ptr_ptr: ['pointer', ['pointer']]
    , str_ptr: ['string', ['pointer']]
    , uint32_ptr: ['uint32', ['pointer']]
    , ptr_ptr_uint32: ['pointer', ['pointer', 'uint32']]
    , int32_ptr_ptr: ['int32', ['pointer', 'pointer']]
    , void_ptr_str: ['void', ['pointer', 'string']]
    , void_ptr_ptr: ['void', ['pointer', 'pointer']]

    };

var libgit2 = new FFI.Library('libgit2', {
      //'git_commit_lookup'         : ['int32', ['pointer', 'pointer', 'pointer']],
      //'git_commit_new'            : ['int32', ['pointer', 'pointer']],
      //'git_commit_time'           : ['time_t', ['pointer']],
      //'git_commit_timezone_offset': ['int32', ['pointer']],
      git_commit_id: type.ptr_ptr
   ,  git_commit_message_short: type.str_ptr
   ,  git_commit_message: type.str_ptr
   ,  git_commit_committer: type.ptr_ptr
   ,  git_commit_author: type.ptr_ptr
   ,  git_commit_tree: type.ptr_ptr
   ,  git_commit_parentcount: type.unit32_ptr
   ,  git_commit_parent: type.ptr_ptr_uint32
   ,  git_commit_add_parent: type.int32_ptr_ptr
   ,  git_commit_set_message: type.void_ptr_str
   ,  git_commit_set_committer: type.void_ptr_ptr
   ,  git_commit_set_author: type.void_ptr_ptr
   ,  git_commit_set_tree: type.void_ptr_ptr
  });

exports.Commit = function(commit) {
  commit = commit instanceof FFI.Pointer ? commit : new FFI.Pointer(FFI.Bindings.POINTER_SIZE);

  this.ccomit = commit;
  this.id = new Oid(libgit2.git_commit_id(commit));
  this.message = libgit2.git_commit_message(commit);
  this.message_short = libgit2.git_commit_message_short(commit);
  this.author = new GITSignature(libgit2.git_commit_author(commit));
  this.committer = new GITSignature(libgit2.git_commit_committer(commit));
};

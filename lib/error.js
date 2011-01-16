var FFI = require('node-ffi');
var libgit2 = new FFI.Library('libgit2', {
  'git_strerror'  : ['string', ['int32']],
});

exports.git_raise_error = function(num) {
  if(num) {
    var e = libgit2.git_strerror(num);
    throw(e);
  }
}

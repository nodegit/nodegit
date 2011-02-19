var FFI = require('node-ffi');
var libgit2 = new FFI.Library('libgit2', {
    'git_reference_new'         : ['int32', ['pointer', 'pointer']],
    'git_reference_oid'         : ['pointer', ['pointer']],
    'git_reference_target'      : ['string', ['pointer']],
    'git_reference_type'        : ['int32', ['pointer']],
    'git_reference_name'        : ['string', ['pointer']],
    'git_reference_resolve'     : ['int32', ['pointer', 'pointer']],
    'git_reference_write'       : ['int32', ['pointer']],
    'git_reference_owner'       : ['pointer', ['pointer']],
    'git_reference_set_name'    : ['void', ['pointer', 'string']],
    'git_reference_set_target'  : ['void', ['pointer', 'string']],
    'git_reference_set_oid'     : ['void', ['pointer', 'pointer']],
})

var Oid = require('./oid').Oid

var git_raise_error = require('./error').git_raise_error;

var Ref = exports.Ref = function(arg, noresolve) {
    var _ref;
    if (arg instanceof FFI.Pointer) {
        _ref = arg
        if(!noresolve) {
            var newref = new FFI.Pointer(FFI.Bindings.POINTER_SIZE)
            var ret = libgit2.git_reference_resolve(newref, _ref)
            git_raise_error(ret)
            _ref = newref.getPointer()
        }
    }
    
    Object.defineProperty(this, "cref",
        { get: function() { return _ref }, enumerable: true });
    Object.defineProperty(this, "oid",
        {
            get: function() {
                return new Oid(libgit2.git_reference_oid(_ref));
            },
            enumerable: true,
        });
    Object.defineProperty(this, "target",
        {
            get: function() { return libgit2.git_reference_target(_ref) },
            enumerable: true,
        });
    Object.defineProperty(this, "name",
        {
            get: function() { return libgit2.git_reference_name(_ref); },
            enumerable: true,
        });
    Object.defineProperty(this, "type",
        {
            get: function() { return libgit2.git_reference_type(_ref); },
            enumerable: true,
        });
}
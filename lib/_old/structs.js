var FFI = require('node-ffi');

var GITTime = new FFI.Struct([
  [ 'uint32', 'time'],
  [ 'int32', 'offset'],
]);

var GITSignatureStruct = new FFI.Struct([
  ['pointer', 'name'],
  ['pointer', 'email'],
  ['uint32', 'time'],
  ['int32', 'offset'],
]);

exports.GITSignature = function(ptr) {
  var _struct = new GITSignatureStruct(ptr);

  Object.defineProperty(this, "name",
    {
      get: function() { return _struct.name.getCString(); },
      enumerable: true,
    });
  Object.defineProperty(this, "email",
    {
      get: function() { return _struct.email.getCString(); },
      enumerable: true,
    });
  Object.defineProperty(this, "time",
    {
      get: function() { return _struct.time; },
      enumerable: true,
    });
  Object.defineProperty(this, "offset",
    {
      get: function() { return _struct.offset; },
      enumerable: true,
    });
}

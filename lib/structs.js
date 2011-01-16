var FFI = require('node-ffi');



exports.GITTime = new FFI.Struct([
  ['time_t', 'time'],
  ['int32', 'offset'],
]);

exports.GITSignature = new FFI.Struct([
  ['string', 'name'],
  ['string', 'email'],
  /* Uhhh this probably won't work */
  ['time_t', 'time'],
  ['int32', 'offset'],
]);


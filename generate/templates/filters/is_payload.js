module.exports = function(field) {
  return field.name == 'payload' || field.payload || field.payloadFor;
};

module.exports = function(fields, payloadForField) {
  fields = fields || [];

  var result = fields.filter(function (field) {
    return field.payloadFor && (field.payloadFor === payloadForField || field.payloadFor === "*");
  });

  if (result.length > 0) {
    return result[0].name;
  }
  else {
    return "";
  }
};

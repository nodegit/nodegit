module.exports = function(fields) {
  var result = [];

  fields.forEach(function (field){
    field.parsedName = field.name || "result";
    field.isCppClassIntType = ~["Uin32", "Int32"].indexOf(field.cppClassName);
    field.parsedClassName = (field.cppClassName || '').toLowerCase() + "_t";
  });

  return result;
};

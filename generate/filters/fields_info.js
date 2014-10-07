module.exports = function(fields) {
  var result = [];

  fields.forEach(function (field){
    var fieldInfo = {};

    fieldInfo.__proto__ = field;

    fieldInfo.parsedName = field.name || "result";
    fieldInfo.isCppClassIntType = ~["Uint32", "Int32"].indexOf(field.cppClassName);
    fieldInfo.parsedClassName = (field.cppClassName || '').toLowerCase() + "_t";

    result.push(fieldInfo);
  });

  return result;
};

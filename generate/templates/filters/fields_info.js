var bannedCppClassNames = [
  "Buffer",
  "Function",
  "GitBuf",
  "GitStrarray",
  "Wrapper"
];

module.exports = function(fields) {
  var result = [];

  fields.forEach(function (field, index){
    var fieldInfo = {};

    fieldInfo.__proto__ = field;

    fieldInfo.index = index;
    fieldInfo.parsedName = field.name || "result";
    fieldInfo.isCppClassIntType = ~["Uint32", "Int32"].indexOf(field.cppClassName);
    fieldInfo.parsedClassName = (field.cppClassName || '').toLowerCase() + "_t";
    fieldInfo.hasOwner = !fieldInfo.selfOwned && !!fieldInfo.ownedByThis;
    fieldInfo.isUnwrappable = fieldInfo.isLibgitType && !fieldInfo.isEnum &&
      !bannedCppClassNames.includes(fieldInfo.cppClassName);

    result.push(fieldInfo);
  });

  return result;
};

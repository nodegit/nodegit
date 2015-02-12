module.exports = function(field) {
  return field.cppClassName == "String" && field.size;
};

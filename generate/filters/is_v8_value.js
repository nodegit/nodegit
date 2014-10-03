var v8 = [
  "Boolean",
  "Number",
  "String",
  "Integer",
  "Int32",
  "Uint32",
  "Date",
  "Function"
];

module.exports = function(cppClassName) {
  return v8.indexOf(cppClassName) > -1;
};

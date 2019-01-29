const isArrayType = require("./is_array_type");

module.exports = function(cType, parsedName) {
  return isArrayType(cType) ?
    "&" + parsedName + "[i]" :
    parsedName;
};

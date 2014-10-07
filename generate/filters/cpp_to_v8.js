var isV8Value = require("./is_v8_value");

module.exports = function(cppClassName) {
  return isV8Value(cppClassName) ? cppClassName : "Object";
};

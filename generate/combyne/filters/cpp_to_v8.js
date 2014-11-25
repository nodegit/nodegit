var isV8Value = require("./is_v8_value");

module.exports = function(cppClassName) {
  if (cppClassName === "Integer") {
    cppClassName = "Number";
  } else if (cppClassName === "Wrapper"){
    cppClassName = "String";
  }

  return isV8Value(cppClassName) ? cppClassName : "Object";
};

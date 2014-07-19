const fs = require("fs");
const path = require("path");

// Make a locally bound path joiner.
var local = path.join.bind(null, __dirname, "../");

exports.read = function(file) {
  try {
    return fs.readFileSync(local(file)).toString();
  }
  catch (unhandledException) {
    return "";
  }
};

exports.write = function(file, contents) {
  try {
    fs.writeFileSync(local(file), contents);
    return true;
  }
  catch (unhandledException) {
    return false;
  }
};

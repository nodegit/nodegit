var path = require("path");

module.exports = function findNodePreGyp() {
  var npg = path.resolve(".", "node_modules", ".bin", "node-pre-gyp");
  return npg.replace(/\s/g, "\\$&");
};

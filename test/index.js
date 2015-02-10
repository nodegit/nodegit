var fork = require("child_process").fork;
var path = require("path");
var local = path.join.bind(path, __dirname);

var args = [
  "cover",
  process.platform != "win32" ?
    "_mocha" :
    local("../node_modules/mocha/bin/_mocha"),
  "--",
  "runner",
  "tests",
  "--report=lcov",
  "--expose-gc"
];

fork(local("../node_modules/istanbul/lib/cli.js"), args, {
  cwd: __dirname
}).on("close", function(code) {
  process.exit(code);
});

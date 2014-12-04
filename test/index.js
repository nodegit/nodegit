var args = [
    "cover",
    process.platform != "win32" ? "_mocha" : "../node_modules/mocha/bin/_mocha",
    "--",
    "runner",
    "tests",
    "--report=lcov",
    "--expose-gc"
  ];

require("child_process").fork("../node_modules/istanbul/lib/cli.js", args, {
  cwd: __dirname
}).on("close", function(code) {
  process.exit(code);
});

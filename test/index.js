var args = "cover _mocha -- runner tests --report=lcov".split(" ");

require("child_process").fork("../node_modules/istanbul/lib/cli.js", args, {
  cwd: __dirname,
  execArgv: [
    process.platform !== "win32" ? "--expose-gc" : ""
  ]
});

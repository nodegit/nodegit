var nodePreGypConstructor = require("node-pre-gyp");
var nodePreGyp = new nodePreGypConstructor.Run();
var buildFlags = require("../utils/buildFlags");

module.exports = function install() {
  // we need to add 2 blank entires to help the parser later.
  var argv = ["", "", "install"];

  if (buildFlags.mustBuild) {
    argv.push("--build-from-source");

    if (buildFlags.debugBuild) {
      argv.push("--debug");
    }
  }
  else {
    argv.push("--fallback-to-build");
  }

  nodePreGyp.parseArgv(argv);

  function run() {
    var command = nodePreGyp.todo.shift();
    if (!command) {
      return;
    }

    nodePreGyp.commands[command.name](command.args, function (err) {
      if (err) {
        console.error(command.name + " error");
        console.error("stack", err.stack);
        console.error("not ok");
        console.log(err.message);
        return process.exit(1);
      }
      var args_array = [].slice.call(arguments, 1);
      if (args_array.length) {
        console.log.apply(console, args_array);
      }
      // now run the next command in the queue
      process.nextTick(run);
    });
  }

  run();
};

// Called on the command line
if (require.main === module) {
  module.exports();
}

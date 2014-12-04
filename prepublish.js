var exec = require('child_process').exec;
try {
  require("./build/Release/nodegit");
  console.info("[nodegit] Nothing to do.")
}
catch (e) {
  exec("node generate");
}

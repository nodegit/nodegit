var exec = require("./exec");
var determineTarget = require("./determineTarget");

return determineTarget()
  .then(function(targetInfo) {
    console.info("[nodegit] Publishing native " +
                 targetInfo.target + " module.");
    return exec("node-pre-gyp publish " + targetInfo.args);
  })
  .then(function() {
    console.info("[nodegit] Publishing completed successfully.");
    process.exitCode = 0;
  },
  function(err, stderr) {
    console.error(err);
    console.error(stderr);
    process.exitCode = 13;
  });

var exec = require("./exec");
var determineTarget = require("./determineTarget");
var findNodePreGyp = require("./findNodePreGyp");

return determineTarget()
  .then(function(targetInfo) {
    var npg = findNodePreGyp();
    console.info("[nodegit] Publishing native " +
                 targetInfo.target + " module.");
    return exec(npg + " publish " + targetInfo.args);
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

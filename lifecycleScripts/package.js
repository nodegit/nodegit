var determineTarget = require("./determineTarget");
var build = require("./build");

return determineTarget()
  .then(function(targetInfo) {
    console.info("[nodegit] Packaging native " +
                 targetInfo.target + " module.");
    return build("package", targetInfo);
  })
  .then(function() {
    console.info("[nodegit] Packaging complete.");
    console.info("[nodegit] Completed packaging successfully.");
    process.exitCode = 0;
  },
  function(err, stderr) {
    console.error(err);
    console.error(stderr);
    process.exitCode = 13;
  });

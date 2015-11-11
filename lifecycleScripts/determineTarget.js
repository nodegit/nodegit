var whichNativeNodish = require("which-native-nodish");

module.exports = function determineTarget(action) {
  return whichNativeNodish("..")
    .then(function(results) {
      var nwVersion = results.nwVersion || process.env.NWJS_VERSION || null;
      var asVersion = results.asVersion || process.env.ELECTRON_VERSION || null;
      var args = [];
      var target = null;
      if (nwVersion) {
        target = "node-webkit";
        args.push("--target=" + nwVersion);
      } else if (asVersion) {
        target = "electron";
        args.push("--target=" + asVersion);
      }

      if (target) {
        args.push("--runtime=" + target);
      }

      target = target || "node";
      return {target: target, args: args};
    });
};

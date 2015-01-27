var fse = require("fs-extra");
var path = require("path");
var npm = require("npm");
var Promise = require("nodegit-promise");

var rooted = path.join.bind(path, __dirname, "..");
if (fse.existsSync(rooted(".didntcomefromthenpmregistry"))) {
  console.error("[nodegit] We only clean when downloaded from the npm " +
    "registry. Skipping clean.");
  return;
}

npm.load({
  "production": true
}, function() {

  var filesToDelete = [
    "build/Makefile",
    "build/binding.Makefile",
    "build/config.gypi",
    "build/gyp-mac-tool",
    "build/nodegit.target.mk",
    "build/vendor",

    "build/Release/.deps",
    "build/Release/git2.a",
    "build/Release/http_parser.a",
    "build/Release/linker.lock",
    "build/Release/obj.target",
    "build/Release/openssl.a",
    "build/Release/ssh2.a",
    "build/Release/zlib.a",

    "build/Debug/.deps",
    "build/Debug/git2.a",
    "build/Debug/http_parser.a",
    "build/Debug/linker.lock",
    "build/Debug/obj.target",
    "build/Debug/openssl.a",
    "build/Debug/ssh2.a",
    "build/Debug/zlib.a",

    "example",
    "generate",
    "include",
    "lifecycleScripts",
    "src",
    "test",
    "vendor",

    ".astylerc",
    ".editorconfig",
    ".gitingore",
    ".gitmodules",
    ".jshintrc",
    ".npmignore",
    ".travis.yml",
    "appveyor.yml",
    "binding.gyp",
    "TESTING.md"
  ];

  var modulesToPrune = [
    "for-node-webkit",
    "which-native-nodish",
    "fs-extra",
    "node-pre-gyp",
    "npm"
  ];

  console.info("[nodegit] Cleaning up after ourselves.");

  Promise.all([
    cleanFiles(),
    pruneDevDependencies(),
    pruneOtherDependencies()
  ]).then(
    function success() {
      console.info("[nodegit] Done cleaning, enjoy!");
    },
    function failure() {
      console.error("[nodegit] Couldn't finish cleaning.");
    }
  );

  function cleanFiles() {
    console.info("[nodegit] Deleting unneeded files.");
    return new Promise(function(resolve, reject) {
      var failures = filesToDelete.some(function(deletable) {
        try {
          fse.removeSync(rooted(deletable));
          return false;
        }
        catch (e) {
          console.info("[nodegit] Error deleting files. Errored on " +
            rooted(deletable) + ".");
          console.error(e);

          reject(e);
          return true;
        }
      });

      if (!failures) {
        console.info("[nodegit] Finished deleting files.");
        resolve();
      }
    });
  }

  function pruneDevDependencies() {
    console.info("[nodegit] Pruning devdependencies.");
    return new Promise(function (resolve, reject) {
      npm.commands.prune(function(err) {
        if (err) {
          console.error("[nodegit] Failed pruning devdependencies.");
          reject(err);
        }
        else {
          console.info("[nodegit] Successfully pruned devdependencies.");
          resolve();
        }
      });
    });
  }

  function pruneOtherDependencies() {
    console.info("[nodegit] Cleaning unneeded dependencies.");
    return new Promise(function (resolve, reject) {
      npm.commands.uninstall(modulesToPrune, function(err) {
        if (err) {
          console.error("[nodegit] Failed cleaning unneeded dependencies.");
          reject(err);
        }
        else {
          console.info("[nodegit] Successfully cleaned unneeded dependencies.");
          resolve();
        }
      });
    });
  }
});

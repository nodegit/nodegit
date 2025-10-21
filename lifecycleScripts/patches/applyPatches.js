var path = require("path");
var fs = require("fs");
var rootDir = path.join(__dirname, "../..");
var exec = require(path.join(rootDir, "./utils/execPromise"));

/**
 * Apply patches to fix compatibility issues
 */
module.exports = function applyPatches() {
  console.log("[nodegit] Applying compatibility patches");

  var patchesDir = path.join(rootDir, "patches");
  var node22Patch = path.join(patchesDir, "node22-fdopen-fix.patch");

  // Check if the Node.js 22 patch exists and needs to be applied
  if (!fs.existsSync(node22Patch)) {
    console.log("[nodegit] No patches to apply");
    return Promise.resolve();
  }

  // Check if patch is already applied by looking for the comment we add
  var zutilPath = path.join(rootDir, "vendor/libgit2/deps/zlib/zutil.h");
  if (fs.existsSync(zutilPath)) {
    var zutilContent = fs.readFileSync(zutilPath, "utf8");
    if (
      zutilContent.includes(
        "Don't redefine fdopen on macOS - system headers already provide it",
      )
    ) {
      console.log("[nodegit] Node.js 22 compatibility patch already applied");
      return Promise.resolve();
    }
  }

  console.log("[nodegit] Applying Node.js 22 fdopen compatibility patch");

  // Apply the patch
  return exec(`git apply --directory=vendor/libgit2 '${node22Patch}'`, {
    cwd: rootDir,
  })
    .then(function () {
      console.log(
        "[nodegit] Successfully applied Node.js 22 compatibility patch",
      );
    })
    .catch(function (error) {
      console.warn(
        "[nodegit] WARNING - Failed to apply Node.js 22 compatibility patch:",
        error.message,
      );
      console.warn("[nodegit] This may cause build failures on Node.js 22+");
      // Don't fail the build, just warn
      return Promise.resolve();
    });
};

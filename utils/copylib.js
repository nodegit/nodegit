const fse = require("fs-extra");

const srcDir = "lib";
const destDir = "dist";

module.exports = function copylib() {
  fse.copySync(srcDir, destDir, { overwrite: true }, function (err) {
    if (err) {
      console.error(err);
    }
  });
}

if (require.main === module) {
  module.exports();
}

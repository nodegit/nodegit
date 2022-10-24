const fse = require("fs-extra");
const path = require("path");
const NodeGit = require("../../");

const getDirExtFiles = function(dir, ext, done) {
  let results = [];
  fse.readdir(dir, function(err, list) {
    if (err) {
      return done(err);
    }
    let i = 0;
    (function next() {
      let file = list[i++];
      if (!file) {
        return done(null, results);
      }
      file = path.resolve(dir, file);
      fse.stat(file, function(err, stat) {
        if (stat && stat.isDirectory()) {
          getDirExtFiles(file, ext, function(err, res) {
            results = results.concat(res);
            next();
          });
        } else {
          if (path.extname(file) == ".".concat(ext)) {
            results.push(file);
          }
          next();
        }
      });
    })();
  });
};

const getDirFilesToChange = function(dir, ext) {
  return new Promise(function(resolve, reject) {
    getDirExtFiles(dir, ext, function(err, results) {
      if (err) {
        reject(err);
      }
      resolve(results);
    });
  });
};

// Changes the content of files with extension 'ext'
// in directory 'dir' recursively.
// Returns relative file paths
const changeDirExtFiles = function (dir, ext, newText) {
  let filesChanged = [];
  return getDirFilesToChange(dir, ext)
  .then(function(filesWithExt) {
    filesWithExt.forEach(function(file) {
      fse.writeFile(
        file,
        newText,
        { encoding: "utf-8" }
      );
      filesChanged.push(path.relative(dir, file));
    });
    return filesChanged;
  })
  .catch(function(err) {
    throw new Error("Error getting files with extension .".concat(ext));
  });
};

// 'times' to limit the number of iterations in the loop.
// 0 means no limit.
const loopingCheckoutHead = async function(repoPath, repo, times) {
  const text0 = "Text0: changing content to trigger checkout";
  const text1 = "Text1: changing content to trigger checkout";

  let iteration = 0;
  for (let i = 0; true; i = ++i%2) {
    const newText = (i == 0) ? text0 : text1;
    const jsRelativeFilePahts = await changeDirExtFiles(repoPath, "js", newText);  // eslint-disable-line
    let checkoutOpts = {
      checkoutStrategy: NodeGit.Checkout.STRATEGY.FORCE,
      paths: jsRelativeFilePahts
    };
    await NodeGit.Checkout.head(repo, checkoutOpts);

    if (++iteration == times) {
      break;
    }
  }
  return;
};

module.exports = loopingCheckoutHead;
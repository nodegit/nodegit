var util = require("util");
var NodeGit = require("../");

// Deprecated -----------------------------------------------------------------

// In 0.28.0 git_error was majorly refactored to have better naming in libgit2
// We will continue to support the old enum entries but with a deprecation
// warning as they will go away soon.
Object.keys(NodeGit.Error.CODE).forEach((key) => {
  Object.defineProperty(NodeGit.Error.CODE, `GITERR_${key}`, {
    get: util.deprecate(
      () => NodeGit.Error.CODE[key],
      `Use NodeGit.Error.CODE.${key} instead of ` +
      `NodeGit.Error.CODE.GETERR_${key}.`
    )
  });
});

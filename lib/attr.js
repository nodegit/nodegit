var NodeGit = require("../");

var Attr = NodeGit.Attr;

Attr.Check = {
  FILE_THEN_INDEX: 0,
  INDEX_THEN_FILE: 1,
  INDEX_ONLY: 2,

  NO_SYSTEM: 1 << 2
};

module.exports = Attr;

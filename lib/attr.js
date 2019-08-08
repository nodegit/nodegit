var util = require("util");
var NodeGit = require("../");

NodeGit.Attr.STATES = {};
var DEPRECATED_STATES = {
  UNSPECIFIED_T: "UNSPECIFIED",
  TRUE_T: "TRUE",
  FALSE_T: "FALSE",
  VALUE_T: "STRING"
};

Object.keys(DEPRECATED_STATES).forEach((key) => {
  const newKey = DEPRECATED_STATES[key];
  Object.defineProperty(NodeGit.Attr.STATES, key, {
    get: util.deprecate(
      () => NodeGit.Attr.VALUE[newKey],
      `Use NodeGit.Attr.VALUE.${newKey} instead of NodeGit.Attr.STATES.${key}.`
    )
  });
});

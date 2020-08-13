var util = require("util");
var NodeGit = require("../");

var Credential = NodeGit.Credential;

var deprecatedFn = (method) =>
  util.deprecate(
    Credential[method].bind(Credential),
    `Use NodeGit.Credential.${method} instead of NodeGit.Cred.${method}`
  );

var createCredTypeDeprecationMessage = type =>
  `Use NodeGit.Credential.TYPE.${type} instead of NodeGit.Cred.TYPE.${type}`;

NodeGit.Cred = {
  defaultNew: deprecatedFn("defaultNew"),
  sshKeyFromAgent: deprecatedFn("sshKeyFromAgent"),
  sshKeyNew: deprecatedFn("sshKeyNew"),
  usernameNew: deprecatedFn("usernameNew"),
  userpassPlaintextNew: deprecatedFn("userpassPlaintextNew"),
  TYPE: Object.keys(Credential.TYPE).reduce(
    (type, key) => {
      Object.defineProperty(type, key, {
        get: util.deprecate(
          () => Credential.TYPE[type],
          createCredTypeDeprecationMessage(type)
        )
      });
      return type;
    },
    {}
  )
};

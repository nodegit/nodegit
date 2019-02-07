var NodeGit = require("../");
var LookupWrapper = NodeGit.Utils.lookupWrapper;

var Reference = NodeGit.Reference;
var Branch = NodeGit.Branch;

/**
* Retrieves the reference by it's short name
* @async
* @param {Repository} repo The repo that the reference lives in
* @param {String|Reference} id The reference to lookup
* @param {Function} callback
* @return {Reference}
*/
Reference.dwim = LookupWrapper(Reference, Reference.dwim);

/**
* Retrieves the reference pointed to by the oid
* @async
* @param {Repository} repo The repo that the reference lives in
* @param {String|Reference} id The reference to lookup
* @param {Function} callback
* @return {Reference}
*/
Reference.lookup = LookupWrapper(Reference);

/**
 * Returns true if this reference is not symbolic
 * @return {Boolean}
 */
Reference.prototype.isConcrete = function() {
  return this.type() == Reference.TYPE.OID;
};

/**
 * Returns if the ref is pointed at by HEAD
 * @return {Boolean}
 */
Reference.prototype.isHead = function() {
  return Branch.isHead(this);
};

/**
 * Returns true if this reference is symbolic
 * @return {Boolean}
 */
Reference.prototype.isSymbolic = function() {
  return this.type() == Reference.TYPE.SYMBOLIC;
};

/**
 * Returns true if this reference is valid
 * @return {Boolean}
 */
Reference.prototype.isValid = function() {
  return this.type() != Reference.TYPE.INVALID;
};

/**
 * Returns the name of the reference.
 * @return {String}
 */
Reference.prototype.toString = function() {
 return this.name();
};

const MAX_NESTING_LEVEL = 10;

const getTerminal = (repo, refName, nesting, prevRef = null) => {
  if (nesting > MAX_NESTING_LEVEL) {
    return Promise.resolve({
      error: NodeGit.Error.CODE.ENOTFOUND,
      out: prevRef
    });
  }

  return NodeGit.Reference.lookup(repo, refName)
    .then((ref) => {
      if (ref.type() === NodeGit.Reference.TYPE.OID) {
        return {
          error: NodeGit.Error.CODE.OK,
          out: ref
        };
      } else {
        return getTerminal(repo, ref.symbolicTarget(), nesting + 1, ref)
          .then(({ error, out }) => {
            if (error === NodeGit.Error.CODE.ENOTFOUND && !out) {
              return { error, out: ref };
            } else {
              return { error, out };
            }
          });
      }
    })
    .catch((error) => {
      return {
        error: error.errno,
        out: null
      };
    });
};

const getSignatureForReflog = (repo) => {
  const { email, name } = repo.ident();
  if (email && name) {
    return Promise.resolve(NodeGit.Signature.now(name, email));
  }

  return NodeGit.Signature.default(repo)
    .catch(() => NodeGit.Signature.now("unknown", "unknown"));
};

Reference.updateTerminal = function (
  repo,
  refName,
  oid,
  signature,
  logMessage
) {
  let signatureToUse;
  let promiseChain = Promise.resolve();

  if (!signature) {
    promiseChain = promiseChain
      .then(() => getSignatureForReflog(repo))
      .then((sig) => {
        signatureToUse = sig;
        return Promise.resolve();
      });
  } else {
    signatureToUse = signature;
  }

  return promiseChain
    .then(() => getTerminal(repo, refName, 0))
    .then(({ error, out }) => {
      if (error === NodeGit.Error.CODE.ENOTFOUND && out) {
        return NodeGit.Reference.create(
          repo,
          out.symbolicTarget(),
          oid,
          0,
          logMessage
        );
      } else if (error === NodeGit.Error.CODE.ENOTFOUND) {
        return NodeGit.Reference.create(
          repo,
          refName,
          oid,
          0,
          logMessage
        );
      } else {
        return NodeGit.Reference.createMatching(
          repo,
          out.name(),
          oid,
          1,
          out.target(),
          logMessage
        );
      }
    })
    .then(() => NodeGit.Reflog.read(repo, refName))
    .then((reflog) => {
      // We may want some kind of transactional logic for this
      // There is a theoretical timing issue that could result in updating
      // the wrong reflog
      reflog.drop(0, 1);
      reflog.append(oid, signatureToUse, logMessage);
      return reflog.write();
    });
};

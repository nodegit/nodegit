var NodeGit = require("../");
var Rebase = NodeGit.Rebase;
var normalizeOptions = NodeGit.Utils.normalizeOptions;
var shallowClone = NodeGit.Utils.shallowClone;

var _init = Rebase.init;
var _open = Rebase.open;
var _abort = Rebase.prototype.abort;
var _commit = Rebase.prototype.commit;

function defaultRebaseOptions(options, checkoutStrategy) {
  let checkoutOptions;
  let mergeOptions;

  if (options) {
    options = shallowClone(options);
    checkoutOptions = options.checkoutOptions;
    mergeOptions = options.mergeOptions;
    delete options.checkoutOptions;
    delete options.mergeOptions;

    if (options.signingCb) {
      let signingCb = options.signingCb;
      options.signingCb = function (
        signatureBuf,
        signatureFieldBuf,
        commitContent
      ) {
        try {
          const signingCbResult = signingCb(commitContent);

          return Promise.resolve(signingCbResult)
            .then(function({ code, field, signedData }) {
              if (code === NodeGit.Error.CODE.OK) {
                signatureBuf.setString(signedData);
                if (field) {
                  signatureFieldBuf.setString(field);
                }
              }

              return code;
            })
            .catch(function(error) {
              if (error && error.code) {
                return error.code;
              }
              return NodeGit.Error.CODE.ERROR;
            });
        } catch (error) {
          if (error && error.code) {
            return error.code;
          }
          return NodeGit.Error.CODE.ERROR;
        }
      };
    }

    options = normalizeOptions(options, NodeGit.RebaseOptions);
  } else {
    options = normalizeOptions({}, NodeGit.RebaseOptions);
    if (checkoutStrategy) {
      checkoutOptions = {
        checkoutStrategy: checkoutStrategy
      };
    }
  }

  if (checkoutOptions) {
    options.checkoutOptions = normalizeOptions(
      checkoutOptions,
      NodeGit.CheckoutOptions
    );
  }

  if (mergeOptions) {
    options.mergeOptions = normalizeOptions(
      mergeOptions,
      NodeGit.MergeOptions
    );
  }

  return options;
}

// Save options on the rebase object. If we don't do this,
// the options may be cleaned up and cause a segfault
// when Rebase.prototype.commit is called.
const lockOptionsOnRebase = (options) => (rebase) => {
  Object.defineProperty(rebase, "options", {
    value: options,
    writable: false
  });
  return rebase;
};

/**
 * Initializes a rebase
 * @async
 * @param {Repository} repo The repository to perform the rebase
 * @param {AnnotatedCommit} branch The terminal commit to rebase, or NULL to
 *                                 rebase the current branch
 * @param {AnnotatedCommit} upstream The commit to begin rebasing from, or NULL
 *                                   to rebase all reachable commits
 * @param {AnnotatedCommit} onto The branch to rebase onto, or NULL to rebase
 *                               onto the given upstream
 * @param {RebaseOptions} options Options to specify how rebase is performed,
 *                                or NULL
 * @return {Remote}
 */
Rebase.init = function(repository, branch, upstream, onto, options) {
  options = defaultRebaseOptions(
    options,
    NodeGit.Checkout.STRATEGY.FORCE
  );
  return _init(repository, branch, upstream, onto, options)
    .then(lockOptionsOnRebase(options));
};

/**
 * Opens an existing rebase that was previously started by either an invocation
 * of Rebase.open or by another client.
 * @async
 * @param {Repository} repo The repository that has a rebase in-progress
 * @param {RebaseOptions} options Options to specify how rebase is performed
 * @return {Remote}
 */
Rebase.open = function(repository, options) {
  options = defaultRebaseOptions(
    options,
    NodeGit.Checkout.STRATEGY.SAFE
  );
  return _open(repository, options)
    .then(lockOptionsOnRebase(options));
};

Rebase.prototype.commit = function(author, committer, encoding, message) {
  return _commit.call(this, author, committer, encoding, message);
};

Rebase.prototype.abort = function() {
  return _abort.call(this);
};

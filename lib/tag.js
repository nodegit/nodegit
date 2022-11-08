var util = require("util");
var NodeGit = require("../");
var LookupWrapper = NodeGit.Utils.lookupWrapper;
var Tag = NodeGit.Tag;

const signatureRegexesBySignatureType = {
  gpgsig: [
    /-----BEGIN PGP SIGNATURE-----[\s\S]+?-----END PGP SIGNATURE-----/gm,
    /-----BEGIN PGP MESSAGE-----[\s\S]+?-----END PGP MESSAGE-----/gm,
  ],
  x509: [
    /-----BEGIN SIGNED MESSAGE-----[\s\S]+?-----END SIGNED MESSAGE-----/gm,
  ]
};

/**
 * Retrieves the tag pointed to by the oid
 * @async
 * @param {Repository} repo The repo that the tag lives in
 * @param {String|Oid|Tag} id The tag to lookup
 * @return {Tag}
 */
Tag.lookup = LookupWrapper(Tag);

/**
 * @async
 * @param {Repository} repo
 * @param {String} tagName
 * @param {Oid} target
 * @param {Signature} tagger
 * @return {String}
 */
Tag.createBuffer = function(repo, tagName, target, tagger, message) {
  return NodeGit.Object.lookup(repo, target, NodeGit.Object.TYPE.ANY)
    .then((object) => {
      if (!NodeGit.Object.typeisloose(object.type())) {
        throw new Error("Object must be a loose type");
      }

      const id = object.id().toString();
      const objectType = NodeGit.Object.type2String(object.type());
      const lines = [
        `object ${id}`,
        `type ${objectType}`,
        `tag ${tagName}`,
        `tagger ${tagger.toString(true)}\n`,
        `${message}${message.endsWith("\n") ? "" : "\n"}`
      ];
      return lines.join("\n");
    });
};

const deprecatedCreateWithSignatureHelper = util.deprecate(function(repo, oidTarget) {
  return repo.getCommit(oidTarget);
}, "Tag.createWithSignature target should be a Git Object, not Oid");

/**
 * @async
 * @param {Repository} repo
 * @param {String} tagName
 * @param {Object} target
 * @param {Signature} tagger
 * @param {String} message
 * @param {Number} force
 * @param {Function} signingCallback Takes a string and returns a string
 *                                   representing the signed message
 * @return {Oid}
 */
Tag.createWithSignature = async (
  repo,
  tagName,
  target,
  tagger,
  message,
  force,
  signingCallback
) => {
  let targetOid;
  if (!target.id) {
    targetOid = await deprecatedCreateWithSignatureHelper(repo, target);
  } else {
    targetOid = target;
  }

  const tagBuffer = await Tag.createBuffer(repo, tagName, targetOid.id(), tagger, message);
  const { code, signedData } = await signingCallback(tagBuffer);
  switch (code) {
    case NodeGit.Error.CODE.OK: {
      const normalizedEnding = signedData.endsWith("\n") ? "" : "\n";
      const signedTagString = tagBuffer + signedData + normalizedEnding;
      return Tag.createFromBuffer(repo, signedTagString, force);
    }
    case NodeGit.Error.CODE.PASSTHROUGH:
      return Tag.create(
        repo,
        tagName,
        targetOid,
        tagger,
        message,
        force
      );
    default: {
      const error = new Error(
        `Tag.createWithSignature threw with error code ${code}`
      );
      error.errno = code;
      throw error;
    }
  }
};

/**
 * Retrieves the signature of an annotated tag
 * @async
 * @param {String} signatureType
 * @return {String|null}
 */
Tag.prototype.extractSignature = function(signatureType = "gpgsig") {
  const id = this.id();
  const repo = this.repo;
  const signatureRegexes = signatureRegexesBySignatureType[signatureType];
  if (!signatureRegexes) {
    throw new Error("Unsupported signature type");
  }

  return repo.odb().then((odb) => {
    return odb.read(id);
  }).then((odbObject) => {
    const odbData = odbObject.toString();

    for (const regex of signatureRegexes) {
      const matchResult = odbData.match(regex);

      if (matchResult !== null) {
        return matchResult[0];
      }
    }

    throw new Error("this tag is not signed");
  });
};

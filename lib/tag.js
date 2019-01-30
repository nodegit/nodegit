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

/**
 * @async
 * @param {Repository} repo
 * @param {String} tagName
 * @param {Oid} target
 * @param {Signature} tagger
 * @param {String} message
 * @param {Number} force
 * @param {Function} signingCallback Takes a string and returns a string
 *                                   representing the signed message
 * @return {Oid}
 */
Tag.createWithSignature = function(
  repo,
  tagName,
  target,
  tagger,
  message,
  force,
  signingCallback
) {
  let tagBuffer;
  return Tag.createBuffer(repo, tagName, target, tagger, message)
    .then((tagBufferResult) => {
      tagBuffer = tagBufferResult;
      return signingCallback(tagBuffer);
    })
    .then((tagSignature) => {
      const normalizedEnding = tagSignature.endsWith("\n") ? "" : "\n";
      const signedTagString = tagBuffer + tagSignature + normalizedEnding;
      return Tag.createFromBuffer(repo, signedTagString, force);
    });
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
      const matchResult = regex.exec(odbData);

      if (matchResult !== null) {
        return matchResult[0];
      }
    }

    return null;
  });
};

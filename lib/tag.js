var NodeGit = require("../");
var LookupWrapper = NodeGit.Utils.lookupWrapper;
var Tag = NodeGit.Tag;

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

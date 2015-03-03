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

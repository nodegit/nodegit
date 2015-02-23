var git = require("../");
var LookupWrapper = require("./util/lookupWrapper");

var Tag = git.Tag;

/**
* Retrieves the tag pointed to by the oid
* @async
* @param {Repository} repo The repo that the tag lives in
* @param {String|Oid|Tag} id The tag to lookup
* @return {Tag}
*/
Tag.lookup = LookupWrapper(Tag);

module.exports = Tag;

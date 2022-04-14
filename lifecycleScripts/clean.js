var cleanForPublish = require("clean-for-publish"); //eslint-disable-line
var path = require("path");

var location = path.join(__dirname, "..");
cleanForPublish(location, ['.node', '.pdb', '.dll']);

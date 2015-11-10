var cleanForPublish = require("clean-for-publish");
var path = require("path");

var location = path.join(__dirname, "..");
cleanForPublish(location);

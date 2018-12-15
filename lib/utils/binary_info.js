var versioning = require("node-pre-gyp/lib/util/versioning");
var napi = require("node-pre-gyp/lib/util/napi");
var fs = require("fs");
var path = require("path");

function binary_info(platform, arch) {
    var package_json = JSON.parse(
        fs.readFileSync(path.resolve(__dirname,"../../package.json"), "utf8"));
    var options = {
        target_platform: platform,
        target_arch: arch
    };
    var napi_build_version = napi.get_napi_build_version_from_command_args([]);
    var info = versioning.evaluate(package_json, options, napi_build_version);
    return info;
}
module.exports = binary_info;

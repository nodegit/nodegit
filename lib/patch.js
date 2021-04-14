"use strict";

var NodeGit = require("../");
var Patch = NodeGit.Patch;
var normalizeOptions = NodeGit.Utils.normalizeOptions;

var _fromBlobs = Patch.fromBlobs;

Patch.fromBlobs = function (
    old_blob,
    old_as_path,
    new_blob,
    new_as_path,
    opts
) {
    opts = normalizeOptions(opts, NodeGit.DiffOptions);
    return _fromBlobs.call(
        this,
        old_blob,
        old_as_path,
        new_blob,
        new_as_path,
        opts
    );
};

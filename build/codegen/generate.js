const fs = require("fs");
const ejs = require("ejs");
const path = require("path");

var idefs = require("./v0.18.0.json");

var classTemplate = ejs.compile(
  "" + fs.readFileSync(String(__dirname + "/templates/class.cc.ejs")), {
    filename: "class.cc"
  });

var headerTemplate = ejs.compile(
  "" + fs.readFileSync(String(__dirname + "/templates/header.h.ejs")), {
    filename: "header.h"
  });

Object.keys(idefs).forEach(function(keyName) {
  var idef = idefs[keyName];

  if (idef.ignore) {
    return;
  }

  fs.writeFileSync(
    __dirname + "/../../include/" + idef.filename, headerTemplate(idef));

  fs.writeFileSync(
    __dirname + "/../../src/" + path.basename(idef.filename, ".h") + ".cc",
      classTemplate(idef));
});

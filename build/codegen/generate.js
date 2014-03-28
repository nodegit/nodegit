const fs = require("fs");
const ejs = require("ejs");
const path = require("path");

var idefs = require("./v0.20.0.json");

var classTemplate = ejs.compile(
  fs.readFileSync(String(path.resolve("./templates/class.cc.ejs")), {
    filename: "class.cc"
  });

var headerTemplate = ejs.compile(
  fs.readFileSync(String(path.resolve("./templates/header.h.ejs")), {
    filename: "header.h"
  });

Object.keys(idefs).forEach(function(keyName) {
  var idef = idefs[keyName];

  if (idef.ignore) {
    return;
  }

  fs.writeFileSync(
    path.resolve("./include/" + idef.filename), headerTemplate(idef));

  fs.writeFileSync(
    path.resolve("./src/" + path.basename(idef.filename, ".h") + ".cc"),
      classTemplate(idef));
});

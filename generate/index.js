// https://github.com/nodegit/nodegit/issues/94
const fs = require("fs");
const ejs = require("ejs");
const path = require("path");
const idefs = require("./idefs");


var local = path.join.bind(null, __dirname);

var classTemplate = ejs.compile(
  "" + fs.readFileSync(local("templates/class.cc.ejs")), {
    filename: "class.cc"
  });

var headerTemplate = ejs.compile(
  "" + fs.readFileSync(local("templates/header.h.ejs")), {
    filename: "header.h"
  });

Object.keys(idefs).forEach(function(keyName) {
  var idef = idefs[keyName];

  if (idef.ignore) {
    return;
  }

  fs.writeFileSync(local("../include/", idef.filename), headerTemplate(idef));

  fs.writeFileSync(
    local("../src/", path.basename(idef.filename, ".h")) + ".cc",
      classTemplate(idef));
});

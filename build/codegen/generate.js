// https://github.com/nodegit/nodegit/issues/94
const fs = require("fs");
const ejs = require("ejs");
const path = require("path");

var typeMap = require("./types.json");

// Make the emscripten directory first.
try {
  fs.mkdirSync(__dirname + "/../emscripten");
} catch(ex) {}

var idefs = require("./compare");

var emBrowserTemplate = ejs.compile(
  "" + fs.readFileSync(String(__dirname + "/templates/em.browser.js.ejs")), {
    filename: "browser.js"
  });

var emClassTemplate = ejs.compile(
  "" + fs.readFileSync(String(__dirname + "/templates/em.class.js.ejs")), {
    filename: "class.js"
  });

var emNodegitTemplate = ejs.compile(
  "" + fs.readFileSync(String(__dirname + "/templates/em.nodegit.js.ejs")), {
    filename: "nodegit.js"
  });

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

  fs.writeFileSync(
    __dirname + "/../emscripten/" + path.basename(idef.filename, ".h") + ".js",
      emClassTemplate({ ctor: idef, typeMap: typeMap }));
});

fs.writeFileSync(
  __dirname + "/../emscripten/nodegit.js", emNodegitTemplate({ files: idefs }));

fs.writeFileSync(
  __dirname + "/../../browser.js", emBrowserTemplate());

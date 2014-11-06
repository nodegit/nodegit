const fs = require("fs");
const path = require("path");
const utils = require("./utils");
const _ = require("lodash");

var version = require("../package.json").libgit2.version;
var descriptor = require("./descriptor.json");
var libgit2 = require("./v" + version + ".json");
var supplement = require("./libgit2-supplement.json");

// libgit2's docs aren't complete so we'll add in what they're missing here
libgit2.types = libgit2.types.concat(supplement.types);

// Turn the types array into a hashmap of struct values and decorate
// the definitions with required data to build the C++ files
var structs = libgit2.types.reduce(function(hashMap, current) {
  var structName = current[0];
  var structDefOverrides = descriptor[structName] || {};
  var structDef = current[1];

  // currently skipping enums
  if (structDef.type === "enum") {
    return hashMap;
  }

  structDef.cType = structName;

  utils.decorateStruct(structDef);

  structDef.fields.forEach(function (field, index, allFields) {
    utils.decorateField(field, allFields, libgit2);
  });

  _.merge(structDef, structDefOverrides);
  hashMap[structName] = structDef;

  return hashMap;
}, {});

// Do a similar transformation and decoration as we did with the types
var classes = libgit2.groups.reduce(function(hashMap, current) {
  var className = current[0];
  var classDefOverrides = descriptor[className] || {};
  var classDef = {};

  utils.decorateClass(className, classDef, current[1], libgit2);

  _.merge(classDef, classDefOverrides);
  hashMap[className] = classDef;

  return hashMap;
}, {});

// if there are any collisions between structs and classes, classes will
// overwrite the struct and grab their fields

Object.keys(classes).forEach(function (key) {
  var structKey = key.replace("git_", "");

  if (structs[structKey]) {
    classes[key].fields = structs[structKey].fields;
    structs[structKey].ignore = true;
  }
});

var output = [];

Object.keys(structs).forEach(function (key) {
  output.push(structs[key]);
});

Object.keys(classes).forEach(function (key) {
  output.push(classes[key]);
});

// Calculate dependencies
var dependencyLookup = {};

libgit2.types.forEach(function (type) {
  if (type[1].type !== "enum") {
    var dependencyFilename = type[0].replace("git_", "");
    dependencyLookup[dependencyFilename] = dependencyFilename;
  }
});

libgit2.groups.forEach(function (group) {
  var dependencyFilename = group[0].replace("git_", "");
  dependencyLookup[dependencyFilename] = dependencyFilename;
});

output.forEach(function (def) {
  if (def.ignore) {
    return;
  }

  var dependencies = {};
  var addDependencies = function (prop) {
    if (prop.ignore) {
      return;
    }

    var type = utils.normalizeCtype(prop.type || prop.cType).replace("git_", "");
    var dependencyFilename = dependencyLookup[type];

    if (dependencyFilename) {
      dependencies[dependencyFilename] = dependencyFilename;
    }

    (prop.args || []).forEach(addDependencies);

    if (prop.return) {
      addDependencies(prop.return);
    }
  };

  def.fields.forEach(addDependencies);
  def.functions.forEach(addDependencies);

  Object.keys(dependencies).forEach(function (dependencyFilename) {
    def.dependencies.push("../include/" + dependencyFilename + ".h");
  });
});

fs.writeFileSync(path.join(__dirname, "idefs.json"),
  JSON.stringify(output, null, 2));

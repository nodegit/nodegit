const fs = require("fs");
const path = require("path");
const utils = require("./utils");
const _ = require("lodash");

var version = require("../package.json").libgit2.version;
var descriptor = require("./descriptor.json");
var libgit2 = require("./v" + version + ".json");
var supplement = require("./libgit2-supplement.json");

libgit2.types.forEach(function(type) {
  if (supplement.types[type[0]]){
    _.merge(type[1], supplement.types[type[0]]);
  }
});

// libgit2's docs aren't complete so we'll add in what they're missing here
Array.prototype.push.apply(libgit2.types, supplement.new.types);
Array.prototype.push.apply(libgit2.groups, supplement.new.groups);

var output = [];
var groupNames = [];
var dependencyLookup = {};

// reduce all of the groups into a hashmap and a name array for easy lookup
var groups = libgit2.groups.reduce(function(memo, group) {
  var groupName = group[0];

  // Some functions are in the wrong group so we can't just ignore them.
  // We have to completely remove them from one group and manually add them
  // into the other.
  var functionNames = group[1].filter(function(fnName) {
    return !supplement.remove[groupName] ||
      !supplement.remove[groupName].functions ||
      !~supplement.remove[groupName].functions.indexOf(fnName);
  });

  functionNames.typeName = groupName;
  memo[groupName] = functionNames;
  groupNames.push(groupName);
  return memo;
}, {});

// Process each type in the types array into classes and structs and
// decorate the definitions with required data to build the C++ files
libgit2.types.forEach(function(current) {
  var typeName = current[0];
  var typeDefOverrides = descriptor[typeName] || {};
  var typeDef = current[1];

  // skip enums for now
  if (typeDef.type === "enum") {
    return;
  }

  typeDef.cType = typeName;
  typeName = typeName.replace("git_", "");
  typeDef.typeName = typeName;
  dependencyLookup[typeName] = typeName;

  typeDef.isClass = utils.isClass(typeName, groupNames);
  typeDef.isStruct = !typeDef.isClass;

  utils.decoratePrimaryType(typeDef);

  if (typeDef.isClass) {
    typeDef.functions = groups[typeName];
    utils.decorateClass(typeDef);
    groups[typeName] = false;
  }
  else {
    utils.decorateStruct(typeDef);
  }

  output.push(typeDef);
});

// Loop over the groups in case we missed anything (eg the types are missing in the docs);
for (var groupName in groups) {
  var groupDef = groups[groupName];
  if (groupDef === false) {
    continue;
  }

  groupDef = {
    functions: groupDef
  };

  var classDefOverrides = descriptor[groupName] || {};
  groupDef.isClass = true;
  groupDef.isStruct = false;

  groupDef.typeName = groupName;
  dependencyLookup[groupName] = groupName;
  utils.decoratePrimaryType(groupDef);
  utils.decorateClass(groupDef, classDefOverrides);

  output.push(groupDef);
}

// Calculate dependencies
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

if (process.argv[2] != "--documentation") {
  utils.filterDocumentation(output);
}

fs.writeFileSync(path.join(__dirname, "idefs.json"),
  JSON.stringify(output, null, 2));

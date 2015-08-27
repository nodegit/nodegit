const fs = require("fs");
const path = require("path");
const utils = require("./utils");
var _;

var version = require("../../package.json").vendorDependencies.libgit2.version;
var libgit2 = require("../input/v" + version + ".json");
var descriptor = require("../input/descriptor.json");
var supplement = require("../input/libgit2-supplement.json");

module.exports = function generateJson() {
  var helpers = require("./helpers");
  _ = require("lodash");
  // libgit2's docs aren't complete so we'll add in what they're missing here
  libgit2.types.forEach(function(type) {
    if (supplement.types[type[0]]) {
      _.merge(type[1], supplement.types[type[0]]);
    }
  });

  libgit2.groups.forEach(function(group) {
    if (supplement.groups[group[0]]) {
      Array.prototype.push.apply(group[1], supplement.groups[group[0]]);
    }
  });

  Array.prototype.push.apply(libgit2.types, supplement.new.types);
  Array.prototype.push.apply(libgit2.groups, supplement.new.groups);
  _.merge(libgit2.functions, supplement.new.functions);

  var output = [];
  var dependencyLookup = {};
  var types = [];
  var enums = [];

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

    // if we have an empty group then just ignore it
    if (functionNames.length) {
      memo[groupName] = functionNames;
    }

    return memo;
  }, {});


  // Split each type from the array into classes/structs and enums
  // each entry is of type ['name', {definingobject}]
  libgit2.types.forEach(function(current) {
    current[1].typeName = current[0];

    // just log these out to a file for fun
    if (current[1].type === "enum") {
      enums.push(current[1]);
    }
    else {
      types.push(current[1]);
    }
  });

  var previous = "";
  enums = _(enums).sortBy("name").reduce(function(enumMemo, enumerable) {
    if (previous == enumerable.typeName) {
      if (process.env.BUILD_ONLY) {
        console.warn('Duplicate definition for enum ' + enumerable.typeName +
          ". skipped.");
      }
    }
    else if (!enumerable.fields) {
      if (process.env.BUILD_ONLY) {
        console.warn('Incomplete definition for enum ' + enumerable.typeName +
          ". skipped.");
      }
    }
    else {
      enumMemo[enumerable.typeName] = {
        typeName: enumerable.typeName.replace(/^git_/, "").replace(/_t$/, ""),
        type: "enum",
        cType: enumerable.typeName,
        isMask: (/_t$/).test(enumerable.typeName),
        values: enumerable.fields.map(function(field) {
          return {
            name: field.name,
            value: field.value
          }
        })
      };
    }

    previous = enumerable.typeName;
    return enumMemo;
  }, {}).valueOf();

  // decorate the definitions with required data to build the C++ files
  types.forEach(function(typeDef) {
    var typeName = typeDef.typeName;
    typeDef.cType = typeName;
    typeName = typeName.replace("git_", "");
    typeDef.typeName = typeName;
    dependencyLookup[typeName] = typeName;

    typeDef.functions = groups[typeName] || [];
    helpers.decoratePrimaryType(typeDef, enums);

    groups[typeName] = false;

    typeDef.type = typeDef.hasConstructor ? "struct" : "class";

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

    groupDef.type = "class";
    groupDef.cType = (descriptor.types[groupName] || {}).cType || groupDef.cType;

    groupDef.typeName = groupName;
    dependencyLookup[groupName] = groupName;
    helpers.decoratePrimaryType(groupDef, enums);

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

      var type = helpers.normalizeCtype(prop.type || prop.cType).replace("git_", "");
      var dependencyFilename = dependencyLookup[type];

      if (dependencyFilename && dependencyFilename !== def.filename) {
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

    // Additionally provide a friendly name to the actual filename.
    def.name = path.basename(def.filename, ".h");

    def.functions.forEach(function(fn) {
      fn.cppClassName = def.cppClassName;
    });
  });

  // Process enums
  _(enums).forEach(function(enumerable) {
    output.some(function(obj) {
      if (enumerable.typeName.indexOf(obj.typeName) == 0) {
          enumerable.owner = obj.jsClassName;
      }
      else if (enumerable.owner) {
        return true;
      }
    });

    var override = descriptor.enums[enumerable.typeName] || {};

    enumerable.owner = override.owner || enumerable.owner || "Enums";

    enumerable.JsName = enumerable.typeName
      .replace(new RegExp("^" + enumerable.owner.toLowerCase()), "")
      .replace(/^_/, "")
      .toUpperCase();

    enumerable.values.forEach(function(value) {
      value.JsName = value.name
        .replace(/^GIT_/, "")
        .replace(override.removeString || "", "")
        .replace(new RegExp("^" + enumerable.owner.toUpperCase()), "")
        .replace(/^_/, "")
        .replace(new RegExp("^" + enumerable.JsName), "")
        .replace(/^_/, "")
        .toUpperCase();

      if (override.values && override.values[value.name]) {
        _.merge(value, override.values[value.name]);
      }
    });

    _.merge(enumerable, _.omit(override, ["values"]));

    output.push(enumerable);
  }).value();

  output = _.sortBy(output, "typeName");

  if (process.argv[2] != "--documentation") {
    helpers.filterDocumentation(output);
  }


  utils.writeFile("output/idefs.json", output);

};

if (require.main === module) {
  module.exports();
}

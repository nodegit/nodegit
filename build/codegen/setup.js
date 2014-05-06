const fs = require("fs");
const path = require("path");

var version = require("../../package.json").libgit2.version;
var descriptor = require("./descriptor.json");
var libgit2 = require("./v" + version + ".json");

var typeMap = require("./types.json");

// Structs or new types between v0.18 and v0.20.
typeMap.__proto__ = {
  "git_filter_mode_t": { cpp: "Number", js: "Number" },
  "const git_blame_hunk*": { cpp: "GitBlameHunk", js: "BlameHunk" },
  "git_filter *": { cpp: "GitFilter", js: "Filter" },
  "const git_status_entry *": { cpp: "StatusEntry", js: "StatusEntry" },
  "const git_index_name_entry *": {
    cpp: "IndexNameEntry", js: "IndexNameEntry" },

  // unsure
  "uint16_t": { cpp: "Integer", js: "Number" },
  "git_blame_options": { cpp: "Integer", js: "Number" },
  "git_blame_options *": { cpp: "Integer", js: "Number" },
  "git_buf *": { cpp: "Buf", js: "Buf" },
  "git_branch_iterator *": { cpp: "BranchIterator", js: "Iterator" },
  "git_branch_iterator **": { cpp: "BranchIterator", js: "Iterator" },
  "git_branch_t *": { cpp: "GitBranch", js: "Branch" }
};

var files = [];

function titleCase(str) {
  return str.split(/_|\//).map(function(val, index) {
    if (val.length) {
      return val[0].toUpperCase() + val.slice(1);
    }

    return val;
  }).join("");
}

function camelCase(str) {
  return str.split(/_|\//).map(function(val, index) {
    if (val.length) {
      return index >= 1 ? val[0].toUpperCase() + val.slice(1) : val;
    }

    return val;
  }).join("");
}

Object.keys(descriptor).forEach(function(fileName, index) {
  var file = descriptor[fileName];

  file.filename = fileName + ".h";
  file.jsClassName = file.cppClassName;

  if (file.jsClassName.indexOf("Git") === 0) {
    file.jsClassName = file.jsClassName.slice(3);
  }

  // Add to the type's list if it's new.
  typeMap["const " + "git_" + fileName + " *"] =
  typeMap["const " + "git_" + fileName + " **"] =
  typeMap["git_" + fileName + " *"] =
  typeMap["git_" + fileName + " **"] =
  typeMap["git_" + fileName] = {
    cpp: file.cppClassName,
    js: file.jsClassName 
  };

  file.functions = libgit2.files[index].functions.map(function(functionName) {
    var funcDescriptor = libgit2.functions[functionName];
    var descriptor = {};
    var cType = file.cType || "";

    if (!functionName || !funcDescriptor) { return; }

    descriptor.cFunctionName = functionName;

    var trimmedName = functionName.slice(cType.length + 1);

    descriptor.cppFunctionName = titleCase(trimmedName);
    descriptor.jsFunctionName = camelCase(trimmedName);

    // Add to the type's list if it's new.
    typeMap["const " + descriptor.cFunctionName + " *"] =
    typeMap[descriptor.cFunctionName + " *"] =
    typeMap[descriptor.cFunctionName] = {
      cpp: descriptor.cppFunctionName,
      js: descriptor.jsFunctionName 
    };

    var retVal = descriptor.return = {};
    retVal.ctype = funcDescriptor.return.type;

    var type = typeMap[retVal.ctype];
    retVal.cppClassName = type.cpp;
    retVal.jsClassName = type.js;

    var args = descriptor.args || [];
    var typeDescriptor = args.length ? args[0].type : "";
    var isCtor = typeDescriptor.indexOf("**") === typeDescriptor.length - 2;
    descriptor.isConstructorMethod = isCtor;

    // Set the prototype method argument.
    descriptor.isPrototypeMethod = !descriptor.isConstructorMethod;

    descriptor.args = funcDescriptor.args.map(function(arg) {
      if (typeMap[arg.type]) {
        return {
          name: arg.name,
          cType: arg.type,
          cppClassName: typeMap[arg.type].cpp,
          jsClassName: typeMap[arg.type].js,
          comment: arg.comment
        };
      }

      return {};
    });

    return descriptor;
  });

  files.push(file);
});

fs.writeFileSync(path.join(__dirname, "idefs.json"), 
  JSON.stringify(files, null, 2));

const fs = require("fs");
const path = require("path");

var version = require("../package.json").libgit2.version;
var descriptor = require("./descriptor.json");
var libgit2 = require("./v" + version + ".json");

var structs = libgit2.types.reduce(function(memo, current) {
  return memo[current[0]] = current[1], memo;
}, {});

// Extracted types.
var typeMap = require("./types.json");

// Structs or new types between v0.18 and v0.20.
typeMap.__proto__ = {
  "git_filter_mode_t": { cpp: "Number", js: "Number" },
  "const git_blame_hunk *": { cpp: "GitBlameHunk", js: "BlameHunk" },
  "git_blame_hunk *": { cpp: "GitBlameHunk", js: "BlameHunk" },
  "git_filter *": { cpp: "GitFilter", js: "Filter" },
  "const git_status_entry *": { cpp: "StatusEntry", js: "StatusEntry" },
  "const git_index_name_entry *": {
    cpp: "IndexNameEntry", js: "IndexNameEntry" },
  "git_time": { cpp: "GitTime", js: "Time", },
  "git_vector": { cpp: "Array", js: "Array", },

  // unsure
  "uint16_t": { cpp: "Integer", js: "Number" },
  "git_blame_options": { cpp: "Integer", js: "Number" },
  "git_blame_options *": { cpp: "Integer", js: "Number" },
  "git_buf *": { cpp: "Buf", js: "Buf" },
  "git_branch_iterator *": { cpp: "BranchIterator", js: "Iterator" },
  "git_branch_iterator **": { cpp: "BranchIterator", js: "Iterator" },
  "git_branch_t *": { cpp: "GitBranch", js: "Branch" },
  "const git_commit *[]": { cpp: "Array", js: "Array" },
  "git_diff_file": { cpp: "GitDiffFile", js: "DiffFile" },
  "git_strarray": { cpp: "Array", js: "Array" },
  "git_diff_notify_cb": { cpp: "GitDiffNotifyCb", js: "DiffNotifyCb" },
  "unsigned char [20]": { cpp: "Array", js: "Array" },
  "git_filter_init_fn": { cpp: "Function", js: "Function" },
  "git_filter_shutdown_fn": { cpp: "Function", js: "Function" },
  "git_filter_check_fn": { cpp: "Function", js: "Function" },
  "git_filter_apply_fn": { cpp: "Function", js: "Function" },
  "git_filter_cleanup_fn": { cpp: "Function", js: "Function" },
  "const git_checkout_options *": { cpp: "GitCheckoutOptions", js: "CheckoutOptions" },
  "git_checkout_notify_cb": { cpp: "Function", js: "Function" },
  "git_checkout_progress_cb": { cpp: "Function", js: "Function" },
  "git_cherry_pick_options *": { cpp: "GitCherryPickOptions", js: "CherryPickOptions" },
  "const git_cherry_pick_options *": { cpp: "GitCherryPickOptions", js: "CherryPickOptions" },
  "const git_merge_options *": { cpp: "GitMergeOptions", js: "MergeOptions" },
  "git_checkout_options *": { cpp: "GitCheckoutOptions", js: "CheckoutOptions" }
};

typeMap["void *"] = { cpp: "Function", js: "Function" };

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

var fileNames = Object.keys(descriptor);

fileNames.forEach(function(fileName, index) {
  var file = descriptor[fileName];
  var structFile = structs["git_" + fileName] || {};

  // Constants.
  file.filename = fileName + ".h";
  file.ignore = typeof file.ignore == "boolean" ? file.ignore : false;
  //FIXME
  file.cppClassName = "Git" + titleCase(fileName);
  file.jsClassName = file.cppClassName;

  // Adjust to find the real the index.
  libgit2.files.some(function(currentFile, _index) {
    if (currentFile.file === file.filename) {
      index = _index;
      return true;
    }
  });

  var cFile = libgit2.files[index];

  if (file.cType === undefined) {
    if (cFile.functions.length) {
      file.cType = cFile.functions[0].split("_").slice(0, 2).join("_");
    }
  }

  if (file.cType) {
    // Add a description if one exists.
    if (structs[file.cType]) {
      file.description = structs[file.cType].description;
    }
    else {
      file.description = "";
    }

    file.freeFunctionName = "git_" + fileName + "_free";
  }

  // TODO Obsolete this.
  if (file.isStruct || !cFile) {
    // No functions.
    cFile = Object.create(file);
    cFile.functions = [];
    cFile.fields = descriptor[fileName].fields || structFile.fields || [];
  }

  // This should turn 'git_clone_options' into 'git_clone_init_options'
  var initFnName = ["git"].concat(fileName.split('_'));

  initFnName.splice(-1, 0, "init");
  initFnName = initFnName.join('_');

  // Does this struct contain an init function? If so then we can create one publicly
  // to pass into functions and other structs
  file.hasConstructor = structFile.used && structFile.used.needs.some(function (fnName) {
    return fnName === initFnName;
  });

  // Doesn't actually exist.
  if (cFile.functions.indexOf(file.freeFunctionName) === -1) {
    delete file.freeFunctionName;
  }


  var legacyFile = {};

  if (file.jsClassName.indexOf("Git") === 0) {
    file.jsClassName = file.jsClassName.slice(3);
  }

  var uniqueTypes = [];

  cFile.functions.forEach(function(functionName) {
    var funcDescriptor = libgit2.functions[functionName];

    var args = funcDescriptor.args || [];

    args.forEach(function(arg) {
      if (!arg.type) { return; }
      if (arg.type.indexOf("git") === 0) {
        var val = arg.type.split(" ")[0].slice(4);

        if (uniqueTypes.indexOf(val) === -1 && descriptor[val]) {
          uniqueTypes.push(val);
        }
      }
    });
  });

  var deps = file.dependencies;

  file.dependencies = uniqueTypes.map(function(file) {
    return "../include/" + file + ".h";
  });

  if (deps) {
    file.dependencies.unshift.apply(file.dependencies, deps);
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

  var fields = file.fields || cFile.fields || (structFile ? structFile.fields || [] : []) || [];

  // Ensure fields is actually an Array.
  if (!Array.isArray(fields)) {
    fields = [];
  }

  // Decorate fields.
  file.fields = fields.map(function(field) {
    try {
      field.cType = field.cType || field.type;
      field.cppFunctionName = titleCase(field.name);
      field.jsFunctionName = camelCase(field.name);
      field.cppClassName = typeMap[field.cType].cpp;
      field.jsClassName = typeMap[field.cType].js;

      if (field.args) {
        field.args.forEach(function(arg) {
          arg.cppClassName = typeMap[arg.cType].cpp;
          arg.jsClassName = typeMap[arg.cType].js;
        });
      }

      var struct = structs[field.cType];

      // This should turn 'git_clone_options' into 'git_clone_init_options'
      var initFnName = field.cType.split('_');

      initFnName.splice(-1, 0, "init");
      initFnName = initFnName.join('_');

      // Does this struct contain an init function? If so then we can create one publicly
      // to pass into functions and other structs
      field.hasConstructor = struct && struct.used && struct.used.needs.some(function (fnName) {
        return fnName === initFnName;
      });

      field.isEnum = struct && struct.type === "enum";
    } catch (ex) {
      console.error(file.filename, field.cType + " is missing a definition.");
    }

    return field;
  });

  // Used to identify a missing function descriptor later on.
  var ident = {};

  // Cache the existing object, before overwriting.
  var functions = file.functions;
  var iterateFunctions = Object.keys(file.functions || {});

  file.functions = [];

  (cFile.functions.length ? cFile.functions : iterateFunctions).forEach(function(functionName, index) {
    if (iterateFunctions.indexOf(functionName) === -1) {
      return;
    }

    var funcDescriptor = libgit2.functions[functionName];
    var descriptor = {};
    var cType = file.cType || "git_" + file.filename.slice(0, -2);

    // From the hand maintained file.
    var functionDescriptor = functions ? functions[functionName] || ident : ident;

    if (!functionName || !funcDescriptor) { return; }

    descriptor.cFunctionName = functionName;
    descriptor.ignore = functionDescriptor === ident;
    descriptor.description = funcDescriptor.description || "";

    if (typeof functionDescriptor.ignore === "boolean") {
      descriptor.ignore = functionDescriptor.ignore;
    }

    var trimmedName = file.trim !== false ? functionName.slice(cType.length + 1) : functionName.slice(4);

    descriptor.cppFunctionName = functionDescriptor.cppFunctionName || titleCase(trimmedName);
    descriptor.jsFunctionName = functionDescriptor.jsFunctionName || camelCase(trimmedName);

    // Add to the type's list if it's new.
    typeMap["const " + descriptor.cFunctionName + " *"] =
    typeMap[descriptor.cFunctionName + " *"] =
    typeMap[descriptor.cFunctionName] = {
      cpp: descriptor.cppFunctionName,
      js: descriptor.jsFunctionName
    };

    var retVal = descriptor.return = functionDescriptor.return || {};
    retVal.cType = funcDescriptor.return.type;
    retVal.comment = funcDescriptor.return.comment;

    var type = typeMap[retVal.cType];

    if (!retVal.cppClassName) {
      retVal.cppClassName = type.cpp;
    }

    if (!retVal.jsClassName) {
      retVal.jsClassName = type.js;
    }

    var args = descriptor.args || [];
    var typeDescriptor = args.length ? args[0].type || "" : "";

    var isCtor = Boolean(functionDescriptor.isConstructorMethod);

    descriptor.isConstructorMethod = isCtor;

    // Set the prototype method argument.
    if (functionDescriptor.isPrototypeMethod !== false) {
      descriptor.isPrototypeMethod = !descriptor.isConstructorMethod;
    }

    var hasReturn = false;

    descriptor.args = funcDescriptor.args.map(function(arg, index) {
      var manualArgs = functionDescriptor.args || [];

      var isSelf = function() {
        if (manualArgs[index] && typeof manualArgs[index].isSelf === "boolean") {
          return manualArgs[index].isSelf;
        }

        if (index === 0) { return false; }
        return file.jsClassName === typeMap[arg.type].js;
      };

      var isOptional = function() {
        if (manualArgs[index] && typeof manualArgs[index].isOptional === "boolean") {
          return manualArgs[index].isOptional;
        }
      };

      var isReturn = function() {
        if (manualArgs[index] && typeof manualArgs[index].isReturn === "boolean") {
          return manualArgs[index].isReturn;
        }

        if (index > 0) { return false; }
        if (arg.name === "out" || arg.name.slice(-4) === "_out") {
          hasReturn = true;
          return true;
        }

        if ((arg.type || "").replace(/[* ]/g, "") === file.cType) {
          hasReturn = true;
          return true;
        }

        return false;
      };

      if (typeMap[arg.type]) {
        var retVal = {
          name: arg.name,
          cType: arg.type,
          cppClassName: typeMap[arg.type].cpp,
          jsClassName: typeMap[arg.type].js,
          comment: arg.comment,
          isReturn: isReturn() || false,
          isSelf: isSelf() || false,
          shouldAlloc: manualArgs[index] ? manualArgs[index].shouldAlloc || false : false
        };

        if (manualArgs[index] && manualArgs[index].arrayElementCppClassName) {
          retVal.arrayElementCppClassName = manualArgs[index].arrayElementCppClassName;
        }

        if (manualArgs[index] && manualArgs[index].cType) {
          retVal.cType = manualArgs[index].cType;
        }

        if (manualArgs[index] && manualArgs[index].additionalCast) {
          retVal.additionalCast = manualArgs[index].additionalCast;
        }

        if (isOptional()) {
          retVal.isOptional = true;
        }

        return retVal;
      }

      throw new Error("Missing typedef for " + arg.type);
    });

    // By default all methods are synchronous.
    descriptor.isAsync = false;

    // Determine if retVal isErrorCode.
    if (retVal.cType === "int" && hasReturn) {
      retVal.isErrorCode = true;
      descriptor.isAsync = true;
    }

    if (typeof functionDescriptor.isAsync === "boolean") {
      descriptor.isAsync = functionDescriptor.isAsync;
    }

    if (Object.keys(functions || {}).indexOf(descriptor.cFunctionName) > -1) {
      descriptor.comment = funcDescriptor.comments || "";
      file.functions.push(descriptor);
    }
  });

  files.push(file);
});

fs.writeFileSync(path.join(__dirname, "types.json"),
  JSON.stringify(typeMap, null, 2));

fs.writeFileSync(path.join(__dirname, "idefs.json"),
  JSON.stringify(files, null, 2));

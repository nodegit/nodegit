var isPointer = require("./is_pointer");

module.exports = function(fn, argReturnsOnly, isAsync) {
  var result = [];
  var args = fn.args || [];

  // We will use this to figure out the index
  // of arguments, because sync functions
  // need to respect ownership to an arbitrary
  // parameter that is labeled by name in the
  // descriptor, and we won't have access to
  // any sort of string to argument index
  // in the template.
  var nameToArgIndex = {};
  var thisArgName = '';
  args.forEach(function (arg, index) {
    nameToArgIndex[arg.name] = index;
    if (arg.isSelf) {
      thisArgName = arg.name;
    }
  });

  args.forEach(function (arg) {
    if (!arg.isReturn) return;

    var return_info = {};

    return_info.__proto__ = arg;

    return_info.isAsync = isAsync;
    return_info.parsedName = isAsync ? "baton->" + return_info.name : return_info.name;
    return_info.isCppClassIntType = ~['Uint32', 'Int32'].indexOf(return_info.cppClassName);
    return_info.needsDereference
      = isAsync &&
        return_info.cppClassName == "Number" &&
        isPointer(return_info.cType);
    return_info.parsedClassName = (return_info.cppClassName || '').toLowerCase() + "_t";
    return_info.returnNameOrName = return_info.returnName || return_info.name;
    return_info.jsOrCppClassName = return_info.jsClassName || return_info.cppClassName;
    return_info.isOutParam = true;
    return_info.hasOwner = !return_info.selfOwned &&
      !!(return_info.ownedBy || return_info.ownedByThis || return_info.ownerFn);
    return_info.ownedByIndices = [];

    if (isAsync && return_info.ownedByThis) {
      return_info.ownedBy = return_info.ownedBy || [];
      return_info.ownedBy.unshift(thisArgName);
    }

    // Here we convert ownedBy, which is the list of parameter names
    // that own this result to the argument index.
    // sync functions will need to know this.
    if (!isAsync && return_info.ownedBy) {
      return_info.ownedBy.forEach(function (argName) {
        return_info.ownedByIndices.push(nameToArgIndex[return_info.ownedBy]);
      })
    }

    result.push(return_info);
  });

  if (!result.length
      && !argReturnsOnly
      && fn.return
      && !fn.return.isErrorCode
      && fn.return.cType != "void") {
    var return_info = {};

    return_info.__proto__ = fn.return;

    return_info.isAsync = isAsync;
    return_info.hasOwner = !return_info.selfOwned &&
      !!(return_info.ownedBy || return_info.ownedByThis || return_info.ownerFn);
    return_info.ownedByIndices = [];
    return_info.ownedBy = return_info.ownedBy || [];

    if (return_info.ownedByThis && isAsync) {
      return_info.ownedBy.push(thisArgName);
    }

    if (!isAsync) {
      return_info.ownedByIndices = return_info.ownedBy.map(function (ownerName) {
        return nameToArgIndex[ownerName];
      });
    }

    return_info.parsedName = return_info.name && isAsync ? "baton->" + return_info.name : "result";
    return_info.isCppClassIntType = ~['Uint32', 'Int32'].indexOf(return_info.cppClassName);
    return_info.parsedClassName = (return_info.cppClassName || '').toLowerCase() + "_t";
    return_info.returnNameOrName = return_info.returnName || return_info.name;
    return_info.jsOrCppClassName = return_info.jsClassName || return_info.cppClassName;

    result.push(return_info);
  }


  return result;
};

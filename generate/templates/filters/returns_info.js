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
  args.forEach(function (arg, index) {
    nameToArgIndex[arg.name] = index;
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
    return_info.hasOwner = !!(return_info.ownedBy || return_info.ownedByThis);
    return_info.ownedByIndex = -1;

    // Here we convert ownedBy, which is the name of the parameter
    // that owns this result to the argument index.
    // sync functions will need to know this.
    if (!isAsync && return_info.ownedBy) {
      return_info.ownedByIndex = nameToArgIndex[return_info.ownedBy];
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
    return_info.hasOwner = !!return_info.ownedByThis;
    return_info.ownedByIndex = -1;
    return_info.parsedName = return_info.name && isAsync ? "baton->" + return_info.name : "result";
    return_info.isCppClassIntType = ~['Uint32', 'Int32'].indexOf(return_info.cppClassName);
    return_info.parsedClassName = (return_info.cppClassName || '').toLowerCase() + "_t";
    return_info.returnNameOrName = return_info.returnName || return_info.name;
    return_info.jsOrCppClassName = return_info.jsClassName || return_info.cppClassName;

    result.push(return_info);
  }


  return result;
};

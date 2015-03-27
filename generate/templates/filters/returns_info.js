var isPointer = require("./is_pointer");

module.exports = function(fn, argReturnsOnly, isAsync) {
  var result = [];
  var args = fn.args || [];

  args.forEach(function (arg) {
    if (!arg.isReturn) return;

    var return_info = {};

    return_info.__proto__ = arg;

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

    result.push(return_info);
  });

  if (!result.length
      && !argReturnsOnly
      && fn.return
      && !fn.return.isErrorCode
      && fn.return.cType != "void") {
    var return_info = {};

    return_info.__proto__ = fn.return;
    return_info.parsedName = return_info.name && isAsync ? "baton->" + return_info.name : "result";
    return_info.isCppClassIntType = ~['Uint32', 'Int32'].indexOf(return_info.cppClassName);
    return_info.parsedClassName = (return_info.cppClassName || '').toLowerCase() + "_t";
    return_info.returnNameOrName = return_info.returnName || return_info.name;
    return_info.jsOrCppClassName = return_info.jsClassName || return_info.cppClassName;

    result.push(return_info);
  }


  return result;
};

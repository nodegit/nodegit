module.exports = function(fn) {
  var result = [];
  var args = fn.args || [];

  args.forEach(function (arg) {
    if (!arg.isReturn) return;

    var return_info = {};

    return_info.__proto__ = arg;

    return_info.parsedName = result.length === 1 ? "baton->" + return_info.name : return_info.name;
    return_info.isCppClassIntType = ~['Uint32', 'Int32'].indexOf(return_info.cppClassName);
    return_info.parsedClassName = (return_info.cppClassName || '').toLowerCase() + "_t";
    return_info.jsNameOrName = return_info.jsName | return_info.name;
    return_info.jsOrCppClassName = return_info.jsClassName || return_info.cppClassName;

    result.push(return_info);
  });

  if (!result.length
      && fn.return
      && !fn.return.isErrorCode
      && fn.return.cType != "void") {
    result.push(fn.return);
  }

  if (result.length === 1) {
    result[0].parsedName = result[0].name ? "baton->" + result[0].name : "result";
  }


  return result;
};

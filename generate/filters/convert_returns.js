module.exports = function(fn) {
  var result = [];
  var args = fn.args || [];

  args.forEach(function (arg) {
    if (!arg.isReturn) return;

    var _return = {};

    _return.__proto__ = arg;

    _return.parsedName = result.length === 1 ? "baton->" + _return.name : _return.name;
    _return.isCppClassIntType = ~['Uint32', 'Int32'].indexOf(_return.cppClassName);
    _return.parsedClassName = (_return.cppClassName || '').toLowerCase() + "_t";
    _return.jsNameOrName = _return.jsName | _return.name;
    _return.jsOrCppClassName = _return.jsClassName || _return.cppClassName;

    result.push(_return);
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

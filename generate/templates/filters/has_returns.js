module.exports = function(fn) {
  var args = fn.args || [];
  var result = args.some(function (arg) {
    return arg.isReturn;
  });

  if (!result
      && fn.return
      && !fn.return.isErrorCode
      && fn.return.cType != "void") {
    result = true;
  }

  return result || (fn.return && fn.return.isErrorCode);
};

module.exports = function(fn) {
  var args = fn.args || [];
  var result = args.reduce(function (currentValue, arg) {
    return currentValue + (arg.isReturn ? 1 : 0);
  }, 0);

  if (!result
      && fn.return
      && !fn.return.isErrorCode
      && fn.return.cType != "void") {
    result = 1;
  }

  return result;
};

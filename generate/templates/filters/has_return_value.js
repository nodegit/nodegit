module.exports = function(fn) {
  if (fn.return
      && !fn.return.isErrorCode
      && fn.return.cType != "void") {
    return true
  }

  return false;
};

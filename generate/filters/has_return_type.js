module.exports = function(functionInfo) {
  if (functionInfo.return) {
    return functionInfo.return.cType != "void" || functionInfo.return.isErrorCode;
  }

  return false;
};

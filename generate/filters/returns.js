module.exports = function(functionInfo) {
  var result = [];

  (functionInfo.args || []).forEach(function(arg) {
    if (arg.isReturn) {
      result.push(arg);
    }
  });

  if (!result.length
      && functionInfo.return
      && !functionInfo.return.isErrorCode
      && functionInfo.return.cType != "void"){
    result.push(functionInfo.return);
  }

  return result;
};

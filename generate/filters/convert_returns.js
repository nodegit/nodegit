module.exports = function(returns) {
  var result = [];

  if (returns.length == 1) {
    var singleReturn = {};

    singleReturn.__proto__ = returns[0];
    singleReturn.parsedName = "baton->" + to.name;
    singleReturn.isCppClassIntType = ~['Uint32', 'Int32'].indexOf(singleReturn.cppClassName);
    singleReturn.parsedClassName = (singleReturn.cppClassName || '').toLowerCase() + "_t";

    result.push(singleReturn);
  }
  else {
    returns.forEach(function (_return) {
      _return.parsedName = _return.name;
      _return.isCppClassIntType = ~['Uint32', 'Int32'].indexOf(_return.cppClassName);
      _return.parsedClassName = (_return.cppClassName || '').toLowerCase() + "_t";
      result.push(_return);
    });
  }

  return result;
};

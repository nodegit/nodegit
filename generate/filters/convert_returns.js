module.exports = function(returns) {
  var result = [];


  returns.forEach(function (_return) {
    _return.parsedName = returns.length === 1 ? "baton->" + _return.name : _return.name;
    _return.isCppClassIntType = ~['Uint32', 'Int32'].indexOf(_return.cppClassName);
    _return.parsedClassName = (_return.cppClassName || '').toLowerCase() + "_t";
    _return.jsNameOrName = _return.jsName | _return.name;

    result.push(_return);
  });

  return result;
};

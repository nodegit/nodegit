module.exports = function(returns) {
  var result = [];

  if (returns.length == 1) {
    var singleReturn = {};

    singleReturn.__proto__ = returns[0];
    singleReturn.name = "baton->" + to.name;

    result.push(singleReturn);
  }
  else {
    returns.forEach(function (_return) {
      result.push(_return);
    });
  }

  return result;
};

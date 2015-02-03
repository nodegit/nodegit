module.exports = function(str) {
  return str.split(/_|\//).map(function(val, index) {
    if (val.length) {
      return val[0].toUpperCase() + val.slice(1);
    }

    return val;
  }).join("");
};

module.exports = function(cType) {
  return /\s\[(\d+)\]\s*/.exec(cType)[1];
};

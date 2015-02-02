module.exports = function(arg) {
  return arg.cppClassName == "GitOid" && !arg.isSelf && !arg.isReturn;
};

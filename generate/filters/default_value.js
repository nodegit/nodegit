module.exports = function(cType) {
  return cType === "git_otype" ? "GIT_OBJ_ANY" : "0";
};

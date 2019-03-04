module.exports = function(args) {
  var result = args.reduce(
    function(argList, arg) {
      var useArg = !arg.isReturn &&
        !arg.isSelf &&
        arg.name !== "payload" &&
        arg.name !== "self" &&
        !arg.ignore;

      if (!useArg) {
        return argList;
      }

      arg.firstArg = argList.length === 0;
      argList.push(arg);

      return argList;
    },
    []
  );

  if (result.length) {
    result[result.length - 1].lastArg = true;
  }

  return result;
};

module.exports = function(args) {
  if (!args) {
    return 0;
  }

  return args.reduce(
    function(count, arg) {
      var shouldCount = !arg.isReturn &&
        !arg.isSelf &&
        arg.name !== "payload" &&
        arg.name !== "self" &&
        !arg.ignore;

      return shouldCount ? count + 1 : count;
    },
    0
  );
};

module.exports = function(args) {
  var cArg,
      jsArg;

  for(cArg = 0, jsArg = 0; cArg < args.length; cArg++) {
    var arg = args[cArg];

    if (!arg.isReturn && !arg.isSelf && !arg.isPayload) {
      jsArg++;
    }
  }

  return jsArg;
};

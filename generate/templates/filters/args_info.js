module.exports = function(args) {
  var result = [],
      cArg,
      jsArg;

  for(cArg = 0, jsArg = 0; cArg < args.length; cArg++) {
    var arg = args[cArg];

    if (!arg.isReturn && !arg.isSelf && !arg.isPayload) {
      arg.isJsArg = true;
      arg.jsArg = jsArg;

      jsArg++;
    }

    if (cArg === args.length -1) {
      arg.lastArg = true;
    }
    else {
      arg.lastArg = false;
    }

    arg.cArg = cArg;
    arg.isCppClassStringOrArray = ~["String", "Array"].indexOf(arg.cppClassName);

    result.push(arg);
  }

  return result;
};

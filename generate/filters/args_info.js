var cppToV8 = require("./cpp_to_v8");

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

    if (cArg >= args.length -1) {
      arg.lastArg = true;
    }
    else {
      arg.lastArg = false;
    }
    
    arg.cArg = cArg;
    arg.v8ValueClassName = cppToV8(arg.cppClassName);

    result.push(arg);
  }

  return result;
};

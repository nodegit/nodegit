var fs = require('fs'),
    _ = require('underscore'),
    path = require('path');

var DOUBLE_POINTER_RE = /\*\*$/;
var OUT_VAR_RE = /_out$/;
function isConstructorMethod(cType, functionInfo) {
  if (!functionInfo.args.length) return false;
  var firstArg = functionInfo.args[0];
  return RegExp(cType + " \\*\\*$").test(firstArg.cType) && !isPrototypeMethod(cType, functionInfo) && !/_free$/.test(functionInfo.cFunctionName);
}

function isPrototypeMethod(cType, functionInfo) {
  if (!functionInfo.args.length) return false;

  var firstArg = functionInfo.args[0];
  return RegExp(cType + " \\*$").test(firstArg.cType) && !/_free$/.test(functionInfo.cFunctionName);
}

var idefs = JSON.parse(fs.readFileSync('v0.18.0.json'));

for (var i in idefs) {
  var idef = idefs[i];

  for (var j in idef.functions) {
    var fn = idef.functions[j];
    var old1 = fn.isPrototypeMethod;
    var old2 = fn.isConstructorMethod;
    fn.isPrototypeMethod = isPrototypeMethod(idef.cType, fn);
    fn.isConstructorMethod = isConstructorMethod(idef.cType, fn);
    fn.isFree = /_free$/.test(fn.cFunctionName);

  }
}
console.log(JSON.stringify(idefs, null, 2));

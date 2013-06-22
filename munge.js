var fs = require('fs'),
    _ = require('underscore'),
    path = require('path');

var idefs = JSON.parse(fs.readFileSync('v0.18.0.json'));

for (var i in idefs) {
  var idef = idefs[i];

  for (var j in idef.functions) {
    var fn = idef.functions[j];
    if (!fn.isConstructorMethod) continue;
    fn.args[0].isReturn = true;
  }
}
console.log(JSON.stringify(idefs, null, 2));

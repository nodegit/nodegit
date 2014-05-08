//n = require("./new.json")
//
//lookup = {}
//
//n.forEach(function(a) {
//  a.functions = a.functions || []
//  a.functions.forEach(function(b) {
//    lookup[b.cFunctionName] = {
//      js: b.jsFunctionName,
//      cpp: b.cppFunctionName
//    }
//
//    b.args.forEach(function(c) {
//      lookup[c.cType] = {
//        js: c.jsClassName,
//        cpp: c.cppClassName
//      };
//    })
//
//    lookup[b.return.cType] = {
//      js: b.return.cppClassName,
//      cpp: b.return.jsClassName
//    }
//  })
//})
//
//console.log(JSON.stringify(lookup, null, 2))
//

types = require("./types.json");
fs = require("fs");

Object.keys(types).forEach(function(typeName) {
  var type = types[typeName];

  if (!type.cpp) { return; }

  if (type.cpp.indexOf("Git") === -1) {
    if (type.cpp[0] === type.cpp[0].toUpperCase()) {
      type.cpp = "Git" + type.cpp;
    }
  }
});

fs.writeFileSync("./types.json", JSON.stringify(types, null, 2));

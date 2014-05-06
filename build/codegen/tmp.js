n = require("./new.json")

lookup = {}

n.forEach(function(a) {
  a.functions = a.functions || []
  a.functions.forEach(function(b) {
    lookup[b.cFunctionName] = {
      js: b.jsFunctionName,
      cpp: b.cppFunctionName
    }

    b.args.forEach(function(c) {
      lookup[c.cType] = {
        js: c.jsClassName,
        cpp: c.cppClassName
      };
    })

    lookup[b.return.cType] = {
      js: b.return.cppClassName,
      cpp: b.return.jsClassName
    }
  })
})

console.log(JSON.stringify(lookup, null, 2))

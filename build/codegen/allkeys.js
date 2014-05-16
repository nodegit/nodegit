var New = require("./new.json");

var all = {};

function recurse(obj) {
  if (typeof obj !== "object") { return; }
  Object.keys(obj).forEach(function(keyName) {
    all[keyName] = true;

    if (typeof obj[keyName] === "object" && !Array.isArray(obj[keyName])) {
      recurse(obj[keyName]);
    } else if (Array.isArray(obj[keyName])) {
      obj[keyName].forEach(recurse);
    }
  });
}

New.forEach(recurse);

console.log(Object.keys(all));

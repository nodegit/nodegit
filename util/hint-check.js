var nodejshint = require('./nodejshint.js').test,
    fs = require('fs'),
    path = require('path');

var files = [];

['lib', 'test', 'example'].forEach(function(dir) {
  console.log(dir);
  fs.readdirSync(dir).forEach(function(file) {
    if (/\.js$/.test(file)) files.push(path.join(dir, file));
  });
});

nodejshint(files, function(failures) {
  console.log(failures, 'failures');

  if(!files.length) {
    process.exit(0);
  } else {
    process.exit(1);
  }
});

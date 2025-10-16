var fork = require("child_process").fork;
var path = require("path");
var fs = require('fs');

var bin = "./node_modules/.bin/nyc";
var cov = [
  "--reporter=lcov",
  "--reporter=text-summary",
  "--report-dir=test/coverage/js",
  "mocha"
]

if (process.platform === 'win32') {
  bin = "./node_modules/mocha/bin/mocha";
  cov = [];
}

var args = cov.concat([
  "test/runner",
  "test/tests",
  "--expose-gc",
  "--timeout",
  "15000"
]);

if (!process.env.APPVEYOR && !process.env.TRAVIS && !process.env.GITHUB_ACTION) {
  var local = path.join.bind(path, __dirname);
  var dummyPath = local("home");
  process.env.HOME = dummyPath;
  process.env.USERPROFILE = dummyPath;
}

// unencrypt test keys
function unencryptKey(fileName) {
  var base64Contents = fs.readFileSync(
    path.join(__dirname, fileName + '.enc'),
    'utf8'
  );
  var asciiContents = Buffer.from(base64Contents, 'base64')
    .toString('ascii');
  fs.writeFileSync(path.join(__dirname, fileName), asciiContents, 'utf8');
}
unencryptKey('id_rsa');
unencryptKey('nodegit-test-rsa');

fork(bin, args, { cwd: path.join(__dirname, "../") }).on("close", process.exit);

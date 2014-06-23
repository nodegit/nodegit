var fs = require('fs');
var rimraf = require('rimraf');
var exec = require('child_process').exec;
var path = require('path');
var async = require('async');

var testFiles = [
  'blob',
  'commit',
  'oid',
 // 'reference',
  'repo',
  'revwalk',
  //'tree_entry',
  //'tree',
];

function setupReposCache(cb) {
  fs.mkdir('repos',function() {
    async.series([
      function empty(cb) { exec('git init repos/empty',function() { cb(); }); },
      function workdir(cb) { exec('git clone https://github.com/nodegit/nodegit.git repos/workdir',function() { cb(); }); },
      function nonrepo(cb) {
        fs.mkdir('repos/nonrepo',function() {
          fs.writeFile('repos/nonrepo/file.txt','This is a bogus file',function() {
            cb();
          });
        });
      }
    ],function() {
      cb();
    });
  });
}

exports.setUp = function(cb) {
  fs.exists('.reposCache', function(exists) {
    if (!exists) {
      setupReposCache(function(err) {
        cb();
      });
    }
  });
};

Object.keys(testFiles).forEach(function(fileName) {
  var testFile = testFiles[fileName]
  exports[testFile] = require('./' + testFile);
});

var fs = require('fs');
var rimraf = require('rimraf');
var ncp = require('ncp').ncp;
var exec = require('child_process').exec;
var path = require('path');
var async = require('async');

var testFiles = ['blob','difflist','oid','repo','tree_entry','commit','reference','revwalk','tree'];

function setupReposCache(cb) {
	fs.mkdir('.reposCache',function() {
		async.series([
			function empty(cb) { exec('git init .reposCache/empty',cb); },
			function workdir(cb) { exec('git clone https://github.com/nodegit/nodegit.git .reposCache/workdir',cb); },
			function nonrepo(cb) {
				fs.mkdir('.reposCache/nonrepo',function() {
					fs.writeFile('.reposCache/nonrepo/file.txt','This is a bogus file',cb);
				});
			}
		],cb);
	});
}

module.exports = {
	setUp: function(cb) {
		fs.exists('.reposCache',function(exists) {
			if (exists) {
				ncp('.reposCache','repos',cb);
			} else {
				setupReposCache(function(err) {
					if (err) { return cb(err); }
					ncp('.reposCache','repos',cb);
				});
			}
		});
	},
	tearDown: function(cb) {
		rimraf('repos',cb);
	}
};

for(var i in testFiles) {
	var testFile = testFiles[i];
	module.exports[testFile] = require('./'+testFile);
}

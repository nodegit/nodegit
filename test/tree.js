var git = require('../'),
    rimraf = require('rimraf'),
    fs = require('fs');

var sha = '5716e9757886eaf38d51c86b192258c960d9cfea';
var fileCount = 512; // Number of blob & blob executabless

exports.walk = function(test) {
  test.expect(515);

  git.Repo.open('repos/workdir/.git', function(error, repo) {
    repo.getCommit(sha, function(error, commit) {
      var entryCount = 0;
      commit.getTree(function(error, tree) {
        tree.walk().on('entry', function(index, entry) {
            test.equals(error, null, 'There should be no error');
            entryCount++;
        }).on('end', function(errors, entries) {
          test.equals(errors, null, 'There should be no error');
          test.equals(entryCount, fileCount, 'The manual tree entry count and the "end" tree entry count do not match');
          test.equals(entries.length, fileCount, 'The end entries count and the manual entry count do not match');
          test.done();
        }).start();
      });
    });
  });
};

exports.insert = function(test) {
  test.expect(1);

  git.Repo.open('repos/workdir/.git', function(error, repo) {
    repo.getCommit(sha, function(error, commit) {
      commit.getTree(function(error, tree) {
        var text = "this is a file\n",
            buffer = new Buffer(text);
        repo.createBlobFromBuffer(buffer, function(error, blobId) {
          var builder = tree.builder();
          builder.insert("lib/baz/bar.txt", blobId, git.TreeEntry.FileMode.Blob);
          builder.write(function(error, treeId) {
            repo.getTree(treeId, function(error, tree) {
              var author = git.Signature.create("Scott Chacon", "schacon@gmail.com", 123456789, 60),
                  committer = git.Signature.create("Scott A Chacon", "scott@github.com", 987654321, 90);
              repo.createCommit(null, author, committer, "message", tree, [commit], function(error, commitId) {
                repo.getCommit(commitId, function(error, commit) {
                  commit.getTree(function(error, tree) {
                    tree.getEntry('lib/baz/bar.txt', function(error, entry) {
                      entry.getBlob(function(error, blob) {
                        test.equals(blob.toString(), text);
                        test.done();
                      });
                    });
                  });
                });
              });
            });
          });
        });
      });
    });
  });
};

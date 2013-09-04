var git = require('../'),
    path = require('path');

// This example opens a certain file, `README.md`, at a particular commit,
// and prints the first 10 lines as well as some metadata.

git.Repo.open(path.resolve(__dirname, '../.git'), function(error, repo) {
  if (error) throw error;

  repo.getCommit('0a70ef1237bc1ea50dbccd395e1a114201b75a68', function(error, commit) {
    if (error) throw error;

    commit.getTree(function(error, tree) {
      if (error) throw error;

      var builder = tree.builder(),
          buffer = new Buffer("this is a file\n");
      
      builder.insertBlob("/lib/baz.txt", buffer, false)
      builder.write(repo, function(error, treeId) {
        if (error) throw error;

        var author = git.Signature.create("Scott Chacon", "schacon@gmail.com", 123456789, 60);
        var committer = git.Signature.create("Scott A Chacon", "scott@github.com", 987654321, 90);

        repo.createCommit(null, author, committer, "message", treeId, [commit], function(error, commitId) {
          console.log("New Commit:", commitId.sha());
        });
      });
    });
  });
});

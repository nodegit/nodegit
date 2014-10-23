var git = require('../'),
    path = require('path'),
    fs = require('fs'),
    fileName = 'newfile.txt',
    fileContent = 'hello world'
    ;

/**
 * This example creates a certain file `newfile.txt`, adds it to the git index and
 * commits it to head. Similar to a `git add newfile.txt` followed by a `git commit`
**/

//open a git repo
git.Repo.open(path.resolve(__dirname, '../.git'), function(openReporError, repo) {
  if (openReporError) throw openReporError;

  //create the file in the repo's workdir
  fs.writeFile(path.join(repo.workdir(), fileName), fileContent, function(writeError) {
    if (writeError) throw writeError;

    //add the file to the index...
    repo.openIndex(function(openIndexError, index) {
      if (openIndexError) throw openIndexError;

      index.read(function(readError) {
        if (readError) throw readError;

        index.addByPath(fileName, function(addByPathError) {
          if (addByPathError) throw addByPathError;

          index.write(function(writeError) {
            if (writeError) throw writeError;

            index.writeTree(function(writeTreeError, oid) {
              if (writeTreeError) throw writeTreeError;

              //get HEAD
              git.Reference.oidForName(repo, 'HEAD', function(oidForName, head) {
                if (oidForName) throw oidForName;

                //get latest commit (will be the parent commit)
                repo.getCommit(head, function(getCommitError, parent) {
                  if (getCommitError) throw getCommitError;
                  var author = git.Signature.create("Scott Chacon", "schacon@gmail.com", 123456789, 60);
                  var committer = git.Signature.create("Scott A Chacon", "scott@github.com", 987654321, 90);

                  //commit
                  repo.createCommit('HEAD', author, committer, 'message', oid, [parent], function(error, commitId) {
                    console.log("New Commit:", commitId.sha());
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

var nodegit = require("../"),
    path = require("path"),
    historyFile = "generate/input/descriptor.json",
    walker,
    historyCommits = [],
    commit,
    repo;

// This code walks the history of the master branch and prints results
// that look very similar to calling `git log` from the command line

function compileHistory(resultingArrayOfCommits) {
  var lastSha;
  if (historyCommits.length > 0) {
    lastSha = historyCommits[historyCommits.length - 1].commit.sha();
    if (
      resultingArrayOfCommits.length == 1 &&
      resultingArrayOfCommits[0].commit.sha() == lastSha
    ) {
      return;
    }
  }

  resultingArrayOfCommits.forEach(function(entry) {
    historyCommits.push(entry);
  });

  lastSha = historyCommits[historyCommits.length - 1].commit.sha();

  walker = repo.createRevWalk();
  walker.push(lastSha);
  walker.sorting(nodegit.Revwalk.SORT.TIME);

  return walker.fileHistoryWalk(historyFile, 500)
    .then(compileHistory);
}

nodegit.Repository.open(path.resolve(__dirname, "../.git"))
  .then(function(r) {
    repo = r;
    return repo.getMasterCommit();
  })
  .then(function(firstCommitOnMaster){
    // History returns an event.
    walker = repo.createRevWalk();
    walker.push(firstCommitOnMaster.sha());
    walker.sorting(nodegit.Revwalk.SORT.Time);

    return walker.fileHistoryWalk(historyFile, 500);
  })
  .then(compileHistory)
  .then(function() {
    historyCommits.forEach(function(entry) {
      commit = entry.commit;
      console.log("commit " + commit.sha());
      console.log("Author:", commit.author().name() +
        " <" + commit.author().email() + ">");
      console.log("Date:", commit.date());
      console.log("\n    " + commit.message());
    });
  })
  .done();

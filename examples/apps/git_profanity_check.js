#!/usr/bin/env node
// vim: ft=javascript

// Copyright 2011-2014, Tim Branyen @tbranyen <tim@tabdeveloper.com>
// Dual licensed under the MIT and GPL licenses.
// Script to detect cursewords in commit messages and provide the offending
// commit sha's.
//
// Usage:
//
//     node git_profanity_check some/repo/.git
//
var git = require("../../");

var curses = ["put", "curse", "words", "here"];
var path = "./.git";
var branch = "master";
var reCurse = new RegExp("\\b(?:" + curses.join("|") + ")\\b", "gi");

// Default path is `.git`.
if (process.argv.length < 3) {
  console.log("No path passed as argument, defaulting to .git.");
}
// Otherwise defaults.
else {
  path = process.argv[2];

  // Set repo branch
  if (process.argv.length < 4) {
    console.log("No branch passed as argument, defaulting to master.");
  }
  else {
    branch = process.argv[3];
  }
}

// Open repository.
git.Repo.open(path)
.then(function(repo) {
  // Open branch, default to master.
  return repo.getBranchCommit(branch);
}).then(function(firstCommit) {
  // Iterate history
  var history = firstCommit.history();

  // Iterate over every commit message and test for words.
  history.on("commit", function(commit) {
    var message = commit.message();

    if (reCurse.test(message)) {
      console.log("Curse detected in commit", commit.sha());
      console.log("=> ", message);
      return;
    }
  });

  // Start history iteration.
  history.start();
});

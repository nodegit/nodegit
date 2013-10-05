#!/usr/bin/env node

// Copyright 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
// Dual licensed under the MIT and GPL licenses.
// Script to detect cursewords in commit messages and provide the
// offending commit sha's.
// vim: ft=javascript
var git = require('../../');

var curses = ['add', 'swears', 'here'],
    path = '../../.git',
    branchName = 'master',
    reCurse = new RegExp('\\b(?:' + curses.join('|') + ')\\b', 'gi');


if (process.argv.length < 3) {
  console.log('No git path passed as argument, defaulting to ./.git');
} else {
  path = process.argv[2];

  if (process.argv.length < 4) {
    console.log('No repo branchName passed as argument, defaulting to master');
  } else {
    branchName = process.argv[3];
  }
}

git.Repo.open(path, function(error, repo) {
  if (error) throw error;

  repo.getBranch(branchName, function(error, branch) { 
    if (error) throw error;

    var history = branch.history();
    history.on('commit', function(commit) {
      if (reCurse.test(commit.message()))
        console.log('Curse detected in commit', commit.sha(), 'message', commit.message());
    }).start();
  });
});

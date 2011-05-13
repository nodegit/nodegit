#!/usr/bin/env node

// Copyright 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
// Dual licensed under the MIT and GPL licenses.
// Script to detect cursewords in commit messages and provide the
// offending commit sha's.
// vim: ft=javascript
var git = require( 'nodegit' );

var curses = [ 'add', 'swears', 'here' ]
  , path = './.git'
  , branch = 'master'
  , wordExp = /\b\w+\b/g;

// Set git path
if ( process.argv.length < 3 ) {
  console.log( 'No path passed as argument, defaulting to ./.git' );
}
else {
  path = process.argv[2];

  // Set repo branch
  if ( process.argv.length < 4 ) {
    console.log( 'No branch passed as argument, defaulting to master' );
  }
  else {
    branch = process.argv[3];
  }
}

// Open repository
git.repo( path, function( err, repo ) {
  if ( err ) {
    throw new Error( err );
  }

  // Open branch
  repo.branch( branch, function( err, branch ) { 
    if ( err ) {
      throw new Error( err );
    }

    // Iterate history
    var history = branch.history();
    history.on( 'commit', function( idx, commit ) {
      // Check commit messages first
      curses.forEach(function( curse ) {
        var messageWords = commit.message.match( wordExp );

        messageWords.forEach(function( word ) {
          if ( word == curse ) {
            console.log( 'Curse detected in commit', commit.sha, 'message' ); 

            return;
          }
        });
      });
    });
  });
});

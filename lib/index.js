var repo = require( './repo.js' ).repo,
    error = require( './error.js' ).error,
    ref = require( './ref.js' ).ref;
    //commit = require( 'commit.js' );

exports.git2 = require( '../build/default/git2.node' );
exports.repo = repo;
exports.ref = ref;
exports.error = error;
//exports.commit = commit.commit;

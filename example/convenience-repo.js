var git = require('../lib');

git.repo( '.git', function( err, path ) {
    console.log( err, path );
});

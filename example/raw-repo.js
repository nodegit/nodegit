var git2 = require( 'nodegit2' ).git2;

var repo = new git2.Repo();

// Access existing repository
repo.open('./.git', function(err, path) {
    console.log(err, path);

    var ref = new git2.Reference();
    repo.lookup_ref( ref, "HEAD", function( err ) {
      console.log( err );
    });
});

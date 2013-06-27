var git = require( '../' ).raw,
    path = require( 'path' );


var repo = new git.Repo();

// Access existing repository
repo.open( path.resolve( '../.git' ), function( err ) {
    var master = new git.Ref(repo);
    repo.lookupRef( master, 'refs/heads/master', function( err, ref ) {
      console.log(err, master);
      var oid = new git.Oid();
      master.oid(oid);
      console.log( oid.toString(40) );
    });
});

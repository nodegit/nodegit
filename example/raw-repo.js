var git2 = require( '../' ).git2;

var repo = new git2.Repo(),
    error = new git2.Error();

// Access existing repository
repo.open('.git', function(err, path) {
    console.log( error.strError(err), path);

    var master = new git2.Ref(repo);
    repo.lookupRef( master, 'refs/heads/master', function( err, ref ) {
      console.log(err, master);
      var oid = new git2.Oid();
      master.oid(oid);
      console.log( oid.toString(40) );
    });
});

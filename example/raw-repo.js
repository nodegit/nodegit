var git2 = require( '../' ).git2;

var repo = new git2.Repo();

// Access existing repository
repo.open('./.git', function(err, path) {
    console.log(err, path);

    var ref = new git2.Ref(repo);
    repo.lookupRef( ref, "head", function( err ) {
      console.log(err);
      var oid = new git2.Oid();
      ref.oid(oid);
      console.log( oid.fmt() );
    });
});

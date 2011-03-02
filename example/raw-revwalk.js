var git = require( '../' ).git2;

var repo = new git.Repo();

// Access existing repository
repo.open( '.git', function( err, path ) {
  var revwalk = new git.RevWalk( repo ),
      oid = new git.Oid(),
      error = new git.Error(),
      master = new git.Ref( repo ),
      commit = new git.Commit( repo );

  if( err ) { console.log( error.strError( err ) ); return; }
  
  oid.mkstr( '7e1fad218e6c0b910c4780b0da111ed5a52dde79' );

  //repo.lookupRef( master, "refs/heads/master", function( err, ref ) {
  //  if( err ) { return; }
  //
  //  var newOid = new git.Oid();
  //  master.oid(newOid);
  //  commit.lookup( repo, newOid, function( err ) {
  //    if( err ) { console.log('Error', error.strError(err)); return; }
  //    console.log(newOid.toString(40));
  //    var _commit = new git.Commit(repo);
  //    function walk() {
  //      revwalk.next(_commit, function( err ) {
  //        if( err ) { console.log(error.strError(err));return; }
  //        console.log( _commit.messageShort() );
  //        walk();
  //      });
  //    }
  //
  //    walk();
  //  });
  //
  //  //var _commit = new git.Commit(repo);
  //
  //});

  commit.lookup( repo, oid, function( err ) {
    if( err ) { console.log('Error', error.strError(err)); return; }
    revwalk.push( commit );

    var _commit = new git.Commit(repo);

    function walk() {
      revwalk.next(_commit, function( err ) {
        if( err ) { return; }
        console.log( _commit.messageShort() );
        walk();
      });
    }

    walk();
  });


  //repo.lookupRef( master, "refs/heads/master", function( err, ref ) {
  //  if( err ) { console.log(error.strError(err)); return; }
  //  var newOid = new git.Oid();
  //  console.log(newOid.toString(40));
  //  commit.lookup( repo, newOid, function( err ) {
  //    console.log( err );
  //    console.log('Test', this);
  //    console.log( error.strError( revwalk.push( this ) ) );
  //  });
  //});
});

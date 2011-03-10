var git = require( '../' ).raw,
    path = require( 'path' );

var repo = new git.Repo();

// Access existing repository
repo.open( path.resolve( '../.git' ), function( err ) {
  var revwalk = new git.RevWalk( repo ),
      oid = new git.Oid(),
      error = new git.Error(),
      master = new git.Ref( repo ),
      commit = new git.Commit( repo );

  if( err ) { console.log( error.strError( err ) ); return; }
  
  oid.mkstr( '2a900f56b6dc6cc285b4d25b2407d9a3dfe76002' );

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
});

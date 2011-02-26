var git2 = require( '../' ).git2;

var repo = new git2.Repo();

// Access existing repository
repo.open('./.git', function(err, path) {
  var revwalk = new git2.RevWalk(repo),
      oid = new git2.Oid(),
      error = new git2.Error(),
      master = new git2.Ref(repo),
      commit = new git2.Commit(repo);

  oid.mkstr('5f2aa9407f7b3aeb531c621c3358953841ccfc98')

  commit.lookup( repo, oid, function( err ) {
    console.log( 'Error: '+ error.strError(err) );
  });


  //repo.lookupRef( master, "refs/heads/master", function( err, ref ) {
  //  console.log(master.toString(40));
  //  if( err ) { console.log(error.strError(err)); return; }
  //  console.log( error.strError( revwalk.push( master.oid() ) ) );
  //});
});

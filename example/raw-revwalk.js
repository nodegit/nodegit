var git2 = require( '../' ).git2;

var repo = new git2.Repo();

// Access existing repository
repo.open('./.git', function(err, path) {
  console.log(err, path);
  var revwalk = new git2.RevWalk(repo),
      oid = new git2.Oid(),
      error = new git2.Error(),
      commit = new git2.Commit(repo);

  oid.mkstr('5f2aa9407f7b3aeb531c621c3358953841ccfc98')

  commit.lookup( repo, oid, function( err, details ) {
    if( err ) { console.log(error.strError(err)); return; }
    console.log( error.strError( revwalk.push( commit ) ) );
  });
});

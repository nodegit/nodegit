var git = require( '../' ).git2;

var repo = new git.Repo();

// Access existing repository
repo.open('.git', function(err, path) {
  var revwalk = new git.RevWalk(repo),
      oid = new git.Oid(),
      error = new git.Error(),
      master = new git.Ref(repo),
      commit = new git.Commit(repo);

  oid.mkstr('75054b7130858db1c1cba13cf8a8febb26b14771')

  commit.lookup( repo, oid, function( err ) {
    console.log( error.strError( err ) );
    console.log( error.strError( revwalk.push( commit ) ) );

    var _commit = new git.Commit(repo);

    function test() {
      revwalk.next(_commit, function( err ) {
        if( err ) { return; }
        console.log( _commit.__proto__ );
        test();
      });
    }

    test();
  });


  //repo.lookupRef( master, "refs/heads/master", function( err, ref ) {
  //  if( err ) { console.log(error.strError(err)); return; }
  //  var newOid = new git.Oid();
  //  console.log(newOid.toString(40));
  //  commit.lookup( repo, newOid, function( err ) {
  //    console.log('Test', this);
  //    console.log( error.strError( revwalk.push( this ) ) );
  //  });
  //});
});

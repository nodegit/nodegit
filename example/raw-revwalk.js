var git = require( '../' ).git2;

var repo = new git.Repo();

// Access existing repository
repo.open('.git', function(err, path) {
  if( err ) { return; }
  
  var revwalk = new git.RevWalk(repo),
      oid = new git.Oid(),
      error = new git.Error(),
      master = new git.Ref(repo),
      commit = new git.Commit(repo);

  oid.mkstr('e6ac1ccb355be1075889413c0a24cf6beda6f747')

  //repo.lookupRef( master, "refs/heads/master", function( err, ref ) {
  //  if( err ) { return; }

  //  var newOid = new git.Oid();
  //  master.oid(newOid);
  //  commit.lookup( repo, newOid, function( err ) {
  //    console.log(newOid.toString(40));
  //    var _commit = new git.Commit(repo);
  //    function walk() {
  //      revwalk.next(_commit, function( err ) {
  //        if( err ) { console.log(error.strError(err));return; }
  //        console.log( _commit.__proto__ );
  //        walk();
  //      });
  //    }

  //    walk();
  //  });

  //  //var _commit = new git.Commit(repo);

  //});

  commit.lookup( repo, oid, function( err ) {
    if( err ) { return; }
    console.log( error.strError( err ) );
    console.log( error.strError( revwalk.push( commit ) ) );
    console.log( commit.timeOffset() );

    var _commit = new git.Commit(repo);

    function walk() {
      revwalk.next(_commit, function( err ) {
        if( err ) { console.log(error.strError(err));return; }
        console.log( _commit.timeOffset() );
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
  //    console.log('Test', this);
  //    console.log( error.strError( revwalk.push( this ) ) );
  //  });
  //});
});

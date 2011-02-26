var git2 = require( '../' ).git2;

var repo = new git2.Repo();

// Access existing repository
repo.open('./.git', function(err, path) {
  console.log(err, path);
  var revwalk = new git2.RevWalk();
  revwalk.alloc(repo, function( err ) {
    console.log(err);
  });
});

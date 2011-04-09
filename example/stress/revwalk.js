var git = require( '../../' ).raw;

//* Stress test revision walking
  setInterval(function() {
    for(var i=0; i<10000; i++) {

      (function() {
        var start = new Date;

        var repo = new git.Repo();
        repo.open( '/home/tim/git/nodegit/.git', function() {
          var oid = new git.Oid();
          oid.mkstr( 'cb76e3c030ab29db332aff3b297dc39451a84762' );

          var commit = new git.Commit( repo );
          commit.lookup( repo, oid, function( err ) {
            var revwalk = new git.RevWalk( repo );
            revwalk.push( commit );

            function walk() {
              var oid = new git.Oid();
              revwalk.next( oid, function( err ) {
                if( !err ) {
                  walk();
                }
              });
            }

            walk();
          } );
        });

      })();

    }
  }, 0);
//*/

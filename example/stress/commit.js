var git = require( '../../' ).raw;

/* Stress test basic commit
  setInterval(function() {
    for(var i=0; i<10000; i++) {
      (function() {

        var start = new Date;

        var repo = new git.Repo();
        repo.open( '/home/tim/git/nodegit/.git', function() {
          var commit = new git.Commit( repo );

          console.log( 'Time taken: ' + (+new Date-start) + 'ms' );
        });

      })();
    }
  }, 0);
//*/


//* Stress test repo open
  //setInterval(function() {
    for(var i=0; i<10000; i++) {

      (function() {
        var start = new Date;

        var repo = new git.Repo();
        repo.open( '/home/tim/git/nodegit/.git', function() {
          var oid = new git.Oid();
          oid.mkstr( 'cb76e3c030ab29db332aff3b297dc39451a84762' );

          var commit = new git.Commit( repo );
          commit.lookup( oid, function( err ) {
            console.log( 'Time taken: ' + (+new Date-start) + 'ms' );
          } );
        });

      })();

    }
  //}, 0);
//*/

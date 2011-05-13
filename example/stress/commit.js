var git = require( '../../' ).raw;

//* Stress test basic commit
  setInterval(function() {
    for(var i=0; i<10000; i++) {
      (function() {

        var start = new Date;

        var repo = new git.Repo();
        repo.open( 'jquery/.git', function() {
          var commit = new git.Commit();

          //console.log( 'Time taken: ' + (+new Date-start) + 'ms' );
        });

      })();
    }
  }, 0);
//*/

//* Stress test repo open
  setInterval(function() {
    for(var i=0; i<10000; i++) {

      (function() {
        var start = new Date;

        var repo = new git.Repo();
        repo.open( 'jquery/.git', function() {
          var oid = new git.Oid();
          oid.mkstr( 'cf702496ee28830f3488ed3f1c3940cfbb2dfa8f' );

          var commit = new git.Commit();
          commit.lookup( repo, oid, function( err ) {
            //console.log( 'Time taken: ' + (+new Date-start) + 'ms' );
          });
        });

      })();

    }
  }, 0);
//*/

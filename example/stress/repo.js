var git = require( 'nodegit' ).raw;

//* Stress test basic repo
  setInterval(function() {
    for(var i=0; i<10000; i++) {

      (function() {
        var start = new Date;
        var repo = new git.Repo();

        //console.log( 'Time taken: ' + (+new Date-start) + 'ms' );
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
        repo.open( '/home/tim/git/nodegit/.git', function() {
        
          //console.log( 'Time taken: ' + (+new Date-start) + 'ms' );
        });
      })();

    }
  }, 0);
//*/

//* Init stress test
  setInterval(function() {
    for(var i=0; i<10000; i++) {

      (function() {
        var start = new Date;
        var repo = new git.Repo();
        repo.init( './test/'+ i +'.git', true, function() { 
          //console.log( 'Time taken: ' + (+new Date-start) + 'ms' );
        });
      })();

    }
  }, 0);
//*/

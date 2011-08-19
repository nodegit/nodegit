//var git = require( '../../' ).raw;
var git = require( 'nodegit' );


//* Stress test revision walking
  //setInterval(function() {
//    for(var i=0; i<10000; i++) {

      (function() {

        git.repo( 'jquery/.git', function() {
          //console.log( 'Repo opened' );

          this.branch( 'master', function() {
            this.history().on( 'commit', function( i, commit ) {
              console.log( commit.id.toString(40) );
            });
          });
        });

        //var repo = new git.Repo();
        //repo.open( 'jquery/.git', function( err ) {

        //  var commit = new git.Commit( repo )
        //    , ref = new git.Ref( repo );

        //  ref.lookup( repo, '/refs/heads/master', function( err ) {
        //    if( err ) { throw new Error( err ); }

        //    var oid = new git.Oid();
        //    ref.oid( oid );


            //commit.lookup( repo, oid, function( err ) {

            //  var revwalk = new git.RevWalk( repo );
            //  revwalk.push( commit );

            //  function walk() {
            //    var _oid = new git.Oid();
            //    revwalk.next( _oid, function( err ) {
            //      if( !err ) {
            //        walk();
            //      }
            //    });
            //  }

            //  walk();

            //});
        //  });
        //});

      })();

//    }
  //}, 0);
//*/

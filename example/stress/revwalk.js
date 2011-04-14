var git = require( '../../' ).raw;

//* Stress test revision walking
  //setInterval(function() {
    //for(var i=0; i<10000; i++) {

      (function() {
        var start = new Date;

        var repo = new git.Repo();
        repo.open( 'jquery/.git', function() {

          var commit = new git.Commit( repo );
            , ref = new git.Ref();


          git.ref( self.repo ).lookup( 'refs/heads/' + name, function( err, ref ) {
          repo.lookup( repo, oid, function( err ) {
            var revwalk = new git.RevWalk( repo );
            revwalk.push( commit );

            function walk() {
              var oid = new git.Oid();
              revwalk.next( oid, function( err ) {
                console.log( new git.Error().strError( err ) );
                if( !err ) {
                  walk();
                }
              });
            }

            walk();
          } );
        });

      })();

    //}
  //}, 0);
//*/

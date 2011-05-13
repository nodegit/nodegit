var git = require( 'nodegit' );

git.repo( 'jquery/.git', function() {
  console.log( 'Repo opened' );

  this.branch( 'master', function() {
    console.log( 'Branch opened' );

    this.history().on( 'commit', function( i, commit ) {
      console.log( commit.id.toString(40) );
    });

  });
});

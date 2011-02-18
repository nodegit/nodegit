var git = require( '../build/default/git2' );

exports.constructor = function( test ){
  test.expect( 2 );
  test.equals( typeof git.Repo, 'function', 'Repo reports as a function.' );
  test.equals( toString.call( git.Repo ), '[object Function]', 'Repo [[Class]] is of type function.' );
  test.done();
};

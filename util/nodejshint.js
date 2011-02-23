var JSHINT = require( './jshint.js' ).JSHINT,
    fs = require( 'fs' );

var nodejshint = function() {
  var counter = 0;

  return function( files, callback ) {
    if( files.length ) {
      var file = files.pop(),
          pass = false;

      fs.readFile( file, function( err, data ) {
        if (err) { throw err; }

        if( pass = JSHINT( data.toString() ), pass ) {
          counter++;
          console.log( '✔ Passed '+ file );
        } else {
          console.log( 'x Failed '+ file );
          JSHINT.errors.forEach( function( err ) {
            
            if( err ) {
              console.log( 'line '+ err.line +'\t', err.reason );
            }
          });
        }

        return nodejshint( files, callback );
      });
    }
    else {
      callback && callback( counter );
      counter = 0;
    }
  };
}();

exports.test = nodejshint;

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

        if( pass = JSHINT( data.toString() ), pass, { laxbreak: true } ) {
          counter++;
          console.log( '[32m✔ Passed [0m'+ file );
        }
        else {
          console.log( '[31mx Failed [0m'+ file );
          JSHINT.errors.forEach( function( err ) {
            
            if( err ) {
              console.log( '[31mline '+ err.line +'\t', err.reason +'[0m' );
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

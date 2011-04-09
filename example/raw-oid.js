var git = require( '../' ).raw;

var oid = new git.Oid();
oid.mkstr('1810DFF58D8A660512D4832E740F692884338CCD');
console.log( oid.toString(40) );

//// Test formatting
//console.log( oid.mkstr('5f2aa9407f7b3aeb531c621c3358953841ccfc98') );
//console.log( oid.toString(40) );

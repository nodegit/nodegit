var git2 = require( '../' ).raw;

var oid = new git2.Oid();
// Valid
console.log( oid.mkstr('1810DFF58D8A660512D4832E740F692884338CCD') );
// Invalid
console.log( oid.mkstr('1838CCD') );

// Test formatting
console.log( oid.mkstr('5f2aa9407f7b3aeb531c621c3358953841ccfc98') );
console.log( oid.toString(40) );

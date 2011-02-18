var git2 = require('../build/default/git2');

// Mkstr
var oid = new git2.Oid();
console.log( oid.mkstr('1810DFF58D8A660512D4832E740F692884338CCD') );
console.log( oid.mkstr('1838CCD') );

// System
var os = require( 'os' );

// Library
var util = require( './util.js' ).util,
    blob = require( './blob.js' ).blob,
    repo = require( './repo.js' ).repo,
    error = require( './error.js' ).error,
    sig = require( './sig.js' ).sig,
    oid = require( './oid.js' ).oid,
    obj = require( './obj.js' ).obj,
    ref = require( './ref.js' ).ref,
    revwalk = require( './revwalk.js' ).revwalk,
    commit = require( './commit.js' ).commit,
    tree = require( './tree.js' ).tree,
    entry = require( './tree_entry.js' ).entry;

// Required for Windows/Cygwin support
var root = [ __dirname, '/../build/default' ].join( '' ), path = process.env.PATH
if( ~os.type().indexOf( 'CYGWIN' ) && !~path.indexOf( root ) ) {
  process.env.PATH = root + ':' + path;
}

// Assign raw api to module
exports.raw = require( '../build/default/nodegit' );

// Assign to module
exports.blob = blob;
exports.util = util;
exports.repo = repo;
exports.ref = ref;
exports.oid = oid;
exports.obj = obj;
exports.sig = sig;
exports.error = error;
exports.revwalk = revwalk;
exports.commit = commit;
exports.tree = tree;
exports.entry = entry;

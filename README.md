NodeJS libgit2 bindings
=======================

Created by Tim Branyen [@tbranyen](http://twitter.com/tbranyen)

Currently under active development, `nodegit2` will provide asynchronous native bindings to the `libgit2` C API.

Building and installing
-----------------------

### Dependancies ###
To run `nodegit2` you will need `NodeJS` and to run unit tests you will need to have `git` installed and accessible from your `PATH` to fetch any `vendor/` addons.

__ Windows Users: __ Compile through Cygwin, following the Unix instructions below.

__ OS X Users: __ Install using `brew` or `macports`.

__ Linux/Unix Users: __ Install from source or your favorite package manager.

### Using NPM ###

The __ easiest __ way to get `nodegit2`

    [tim@thinkpad Projects]$ npm install nodegit2

### Mac OS X/Linux/Unix ###

#### Install `NodeJS` from [http://nodejs.org/](http://nodejs.org/) ####

    [tim@thinkpad Projects]$ cd node-v0.4.0
    [tim@thinkpad node-v0.4.0]$ ./configure
    [tim@thinkpad node-v0.4.0]$ make 
    [tim@thinkpad node-v0.4.0]$ sudo make install

#### Install `nodegit2` by cloning source from __GitHub__ and running the `make` and `make install` commands. ####

    [tim@thinkpad Projects]$ git clone git@github.com:tbranyen/nodegit2.git
    [tim@thinkpad Projects]$ cd nodegit2
    [tim@thinkpad nodegit2]$ make
    [tim@thinkpad nodegit2]$ make install

### Windows via Cygiwn ###

#### `nodegit2` has been compiled and tested to work with the setup required to build and run `NodeJS` itself ####

Instructions on compiling `NodeJS` on a Windows platform can be found here:
[https://github.com/ry/node/wiki/Building-node.js-on-Cygwin-(Windows)](https://github.com/ry/node/wiki/Building-node.js-on-Cygwin-(Windows\))

API Example Usage
-----------------

### Convenience API ###
__ Reading a repository and commit data __

    var git = require('nodegit2');
    
    // Read the current repository
    git.repo( '.git', function( err, path, repo ) {
        // If success will return 0, if an error message throw it as an error string.
        if( !err ) throw err;

        // Read a commit
        this.commit( '5f2aa9407f7b3aeb531c621c3358953841ccfc98', function( err, details, commit ) {
            // If success will return 0, if an error message throw it as an error string.
            if( !err ) throw err;

            console.log( 'Message', details.message );
            console.log( 'Author name', details.author.name );
            console.log( 'Author email', details.author.email );

            // Memory cleanup is *not* required, but would be nice if you remembered :)
            repo.free();
        });
    });

### Raw API ###
__ Accomplishing the same thing as above __

    var git = require('nodegit2').git2;
    
    // Create instance of Repo constructor
    var repo = new git.Repo();
    // Read the current repository
    repo.open( '.git', function( err, path ) {
        // If success will return 0, if an error message throw it as an error string.
        if( !err ) throw err;

        // Create object id and set hash
        var oid = new git.Oid();
        oid.mkstr( '5f2aa9407f7b3aeb531c621c3358953841ccfc98' );

        // Create commit object
        var commit = new git.Commit();

        // Lookup commit
        commit.lookup( repo, oid, function( err, details ) {
            // If success will return 0, if an error message throw it as an error string.
            if( !err ) throw err;

            console.log( 'Message', details.message );
            console.log( 'Author name', details.author.name );
            console.log( 'Author email', details.author.email );

            // Memory cleanup is *not* required, but would be nice if you remembered :)
            repo.free();
        });
    });

Running tests
-------------

__ Ensure the submodules `nodeunit` and `rimraf` are located in the `/vendor` subdirectory. __

If they are not, `cd` into the `nodegit2` dir and run the following git commands to automatically fetch them:
    [tim@thinkpad Projects]$ cd nodegit2
    [tim@thinkpad nodegit2]$ git submodule init
    [tim@thinkpad nodegit2]$ git submodule update 

Then simply run `make unittest` in the project root.

Example of building `nodegit2` bindings and running tests:
    [tim@thinkpad Projects]$ cd nodegit2
    [tim@thinkpad nodegit2]$ make
    [tim@thinkpad nodegit2]$ make unittest 

You will most likely install `nodeunit` and `rimraf` via `npm` or make an alias to the `nodeunit` binary in `/vendor`.

Release information
-------------------

### v0.0.1: ###
    * 1:1 mapping of libgit2 read methods
    * An API that can be easily extended with convenience methods in JS
    * An API that offers a familiar clean syntax that will make adoption and use much more likely

### v0.0.2: ###
    * Write capabilities
    * GitHub landing page

### v0.0.3: ###
    * Custom odb backend
    * API coverage in GitHub Wiki

Getting involved
----------------

If you find this project of interest, please document all issues and fork if you feel you can provide a patch.  Testing is of huge importance; by simply running the unit tests on your system and reporting issues you can contribute!

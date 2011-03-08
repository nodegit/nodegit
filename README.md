Node.js libgit2 bindings
=======================

Created by Tim Branyen [@tbranyen](http://twitter.com/tbranyen)

Currently under active development, `nodegit` provides asynchronous native bindings to the `libgit2` C API.

Building and installing
-----------------------

### Dependancies ###
To run `nodegit` you need `Node.js` and to run unit tests you will need to have `git` installed and accessible from your `PATH` to fetch any `vendor/` addons.

### Easy install (Recommended) ###
This will install and configure everything you need to use `nodegit`.

    $ sudo npm install nodegit

### Mac OS X/Linux/Unix ###

#### Install `nodegit` by cloning source from __GitHub__ and running the `configure`, `make`, and `make install` commands: ####
\*Note: `nodegit` assumes your library path exists at `~/.node_libraries`.\*
    
    $ git clone git://github.com/tbranyen/nodegit.git
    $ cd nodegit
    $ ./configure
    $ make
    $ make install

### Windows via Cygwin ###

#### `nodegit` has been compiled and tested to work with the setup required to build and run `Node.js` itself. ####

Instructions on compiling `Node.js` on a Windows platform can be found here:
[https://github.com/ry/node/wiki/Building-node.js-on-Cygwin-(Windows)](https://github.com/ry/node/wiki/Building-node.js-on-Cygwin-(Windows\))

API Example Usage
-----------------

### Convenience API ###
__ Reading a repository and commit data: __

    var git = require( 'nodegit' );
    
    // Read the current repository
    git.repo( '.git', function( err, repo ) {
        // If success err will be 0, else throw an error message.
        if( err ) { throw err; }

        // Work within the master branch
        repo.branch( 'master', function( err, branch ) {
            // If success err will be 0, else throw an error message.
            if( err ) { throw err; }

            // Iterate over the revision history
            branch.history.each( function( i, commit ) {
                // Emulator git log 
                console.log( 'Author:', commit.author().name, '<' + commit.author().email + '>' );
                console.log( 'Date:', commit.time().toDateString() );
                console.log( commit.message() );
            });
        });

        // Read a commit with a SHA1
        this.commit( '5f2aa9407f7b3aeb531c621c3358953841ccfc98', function( err, commit ) {
            // If success err will be 0, else throw an error message.
            if( err ) { throw err; }

            console.log( 'Message', commit.message );
            console.log( 'Author name', commit.author().name );
            console.log( 'Author email', commit.author().email );

            // Memory cleanup is *not* required, but would be nice if you remembered :)
            repo.free();
        });
    });

### Raw API ###
__ Accomplishing the same thing as above: __

    var git = require( 'nodegit' ).raw;
    
    // Create instance of Repo constructor
    var repo = new git.Repo();
    // Read the current repository
    repo.open( '.git', function( err ) {
        // If success will return 0, if an error message throw it as an error string.
        if( err ) { throw err };

        // Create object id and set hash
        var oid = new git.Oid();
        oid.mkstr( '5f2aa9407f7b3aeb531c621c3358953841ccfc98' );

        // Create commit object
        var commit = new git.Commit();

        // Lookup commit
        commit.lookup( repo, oid, function( err, commit ) {
            // If success err will be 0, else throw an error message.
            if( err ) { throw err; }

            console.log( 'Message', commit.message );
            console.log( 'Author name', commit.author().name );
            console.log( 'Author email', commit.author().email );

            // Memory cleanup is *not* required, but would be nice if you remembered :)
            repo.free();
        });
    });

Running tests
-------------

__ `nodegit` library code is written adhering to a modified `JSHint`. Run these tests with `make lint`. __

__ To run unit tests ensure the submodules `nodeunit` and `rimraf` are located in the `vendor/` subdirectory. __

If they are not, `cd` into the `nodegit` dir and run the following `git` commands to automatically fetch them:
    $ cd nodegit
    $ git submodule init vendor/
    $ git submodule update vendor/

Then simply run `make unittest` in the project root.

Release information
-------------------

__ Can keep track of current method coverage at: [http://bit.ly/tb_methods](http://bit.ly/tb_methods) __

### v0.0.1: ###
    * Some useful methods implemented
    * Some unit tests
    * Some documented source code
    * Useable build/code quality check tools
    * Node.js application that can be configured/built/installed via source and NPM
    * An API that can be easily extended with convenience methods in JS
    * An API that offers a familiar clean syntax that will make adoption and use much more likely
    * Open for public testing
    * Be able to open and read a repo and fetch all its commits and lookup specific commits and their associated metadata + reading blob rawcontent.

### v0.0.2: ###
    * More methods implemented
    * More unit tests
    * GitHub landing page
    * More API development

### v0.0.3: ###
    * Custom odb backend
    * API coverage in GitHub Wiki

Getting involved
----------------

If you find this project of interest, please document all issues and fork if you feel you can provide a patch.  Testing is of huge importance; by simply running the unit tests on your system and reporting issues you can contribute!

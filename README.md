Node.js libgit2 bindings
=======================

Created by Tim Branyen [@tbranyen](http://twitter.com/tbranyen)

Currently under active development (and seeking contributors), `nodegit` provides asynchronous native bindings to the `libgit2` C API.

Building and installing
-----------------------

### Dependancies ###
To run `nodegit` you need `Node.js` and to run unit tests you will need to have `git` installed and accessible from your `PATH` to fetch any `vendor/` addons.

### Easy install (Recommended) ###
This will install and configure everything you need to use `nodegit`.

    $ sudo npm install nodegit

### Mac OS X/Linux/Unix ###

#### Install `nodegit` by cloning source from __GitHub__ and running the `configure`, `make`, and `make install` commands: ####
\*Note: `nodegit` assumes your library path exists at `~/.node_libraries` you can change this by specifying a new path\*
    
    $ git clone git://github.com/tbranyen/nodegit.git
    $ cd nodegit

    $ ./configure
    $ make
    $ make install
    
    $ make install NODE_LIB_PATH=/path/to/your/libraries

\*Updating to a new version\*

    $ make update

    $ make update NODE_LIB_PATH=/path/to/your/libraries

### Windows via Cygwin ###

#### `nodegit` has been compiled and tested to work with the setup required to build and run `Node.js` itself. ####

Instructions on compiling `Node.js` on a Windows platform can be found here:
[https://github.com/ry/node/wiki/Building-node.js-on-Cygwin-(Windows)](https://github.com/ry/node/wiki/Building-node.js-on-Cygwin-(Windows\))

API Example Usage
-----------------

### Git Log Emulation ###

#### Convenience API ####

    var git = require( 'nodegit' );
    
    // Read a repository
    git.repo( '.git', function( err, repo ) {
        // Success is always 0, failure is always an error string
        if( err ) { throw err; }

        // Use the master branch
        repo.branch( 'master', function( err, branch ) {
            if( err ) { throw err; }

            // Iterate over the revision history
            branch.history.each( function( i, commit ) {

                // Print out `git log` emulation
                console.log( 'commit ' + commit.sha );
                console.log( commit.author.name + '<' + commit.author.email + '>' );
                console.log( commit.time );
                console.log( '\n' );
                console.log( commit.message );
                console.log( '\n' );
            });
        });

        // Memory cleanup
        repo.free();
    });

#### Raw API ####

    var git = require( 'nodegit' ).raw;
    
    // Create instance of Repo constructor
    var repo = new git.Repo();

    // Read a repository
    repo.open( '.git', function( err ) {
        // Err is an integer, success is 0, use strError for string representation
        if( err ) {
            var error = new git.Error();
            throw error.strError( err );
        }

        // Create instance of Ref constructor with this repository
        var ref = new git.Ref( repo );
        
        // Find the master branch
        repo.lookupRef( ref, '/refs/heads/master', function( err ) {
            if( err ) {
              var error = new git.Error();
              throw error.strError( err );
            }

            // Create instance of Commit constructor with this repository
            var commit = new git.Commit( repo ),
                // Create instance of Oid constructor
                oid = new git.Oid();

            // Set the oid constructor internal reference to this branch reference
            ref.oid( oid );

            // Lookup the commit for this oid
            commit.lookup( oid, function() {
                if( err ) {
                  var error = new git.Error();
                  throw error.strError( err );
                }

                // Create instance of RevWalk constructor with this repository
                var revwalk = new git.RevWalk( repo );

                // Push the commit as the start to walk
                revwalk.push( commit );

                // Recursive walk
                function walk() {
                    // Each revision walk iteration yields a commit
                    var revisionCommit = new git.Commit( repo );

                    revwalk.next( revisionCommit, function( err ) {
                        // Finish recursion once no more revision commits are left
                        if( err ) { return; }

                        // Create instance of Oid for sha
                        var oid = new git.Oid();

                        // Set oid to the revision commit
                        revisionCommit.id( oid );

                        // Create instance of Sig for author
                        var author = new git.Sig();

                        // Set the author to the revision commit author
                        revisionCommit.author( author );

                        // Convert timestamp to milliseconds and set new Date object
                        var time = new Date( revisionCommit.time() * 1000 );

                        // Print out `git log` emulation
                        console.log( oid.toString( 40 ) );
                        console.log( author.name() + '<' + author.email() + '>' );
                        console.log( time );
                        console.log( '\n' );
                        console.log( revisionCommit.message() );
                        console.log( '\n' );

                        // Recurse!
                        walk();
                    });
                }

                // Initiate recursion
                walk():
            });
        });
    });

Running tests
-------------

__ `nodegit` library code is written adhering to a modified `JSHint`. Run these checks with `make lint` in the project root. __

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
    * GitHub landing page (already done)
    * More API development

### v0.0.3: ###
    * Custom odb backend
    * API coverage in GitHub Wiki

Getting involved
----------------

If you find this project of interest, please document all issues and fork if you feel you can provide a patch.  Testing is of huge importance; by simply running the unit tests on your system and reporting issues you can contribute!

__ Before submitting a pull request, please ensure both unit tests and lint checks pass. __

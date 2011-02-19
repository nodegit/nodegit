NodeJS libgit2 bindings
=======================

Written by Tim Branyen @tbranyen 
with guidance from Tim Fontaine @tjfontaine

Currently under active development, this branch will provide native extension methods to the libgit2 C API.

Building
--------

### Dependancies ###
To use node-libgit2 development tree, you will need to have the `libgit2` api in `/usr/local/lib` and the NodeJS
framework installed, you will also need git installed and accessible from your `PATH` to fetch any vendor addons.

### Linux/Mac OS X/BSD Variants ###
You can skip this step and node-libgit2 will automatically fetch and install a fresh copy of libgit2 for you.

* Install libgit2 from [http://libgit2.github.com/](http://libgit2.github.com/) 

    [tim@thinkpad Projects]$ cd libgit2
    [tim@thinkpad libgit2]$ ./configure
    [tim@thinkpad libgit2]$ make 
    [tim@thinkpad libgit2]$ sudo make install

* Install NodeJS from [http://nodejs.org/](http://nodejs.org/)

    [tim@thinkpad Projects]$ cd node-v0.4.0
    [tim@thinkpad node-v0.4.0]$ ./configure
    [tim@thinkpad node-v0.4.0]$ make 
    [tim@thinkpad node-v0.4.0]$ sudo make install

* Install nodegit2 by running the `make` and `make install` commands.  (Future will be on NPM)

    [tim@thinkpad Projects]$ cd nodegit2
    [tim@thinkpad nodegit2]$ make
    [tim@thinkpad nodegit2]$ make install

### Windows via Cygiwn ###

nodegit2 has been compiled and tested to work with the setup required to build and run NodeJS itself, instructions on compiling NodeJS
on a Windows platform can be found here:
[https://github.com/ry/node/wiki/Building-node.js-on-Cygwin-(Windows)](https://github.com/ry/node/wiki/Building-node.js-on-Cygwin-(Windows))

Example API Usage
-----------------

### Creating and reading a repository ###

    var git = require('git2');
    
    // Create a bare repository in the working directory
    git.repo().init( '.git', true, function( err, path, is_bare ) {
        // Read the current repository
        git.repo( '.git', function( err, path ) {
            // ...
        });
    });


Unit testing
------------

##### New way #####
Ensure the submodules `nodeunit` and `rimraf` are located in the `/vendor` subdirectory.

If they are not, `cd` into the `nodegit2` dir and run the following git commands:
    [tim@thinkpad Projects]$ cd nodegit2
    [tim@thinkpad nodegit2]$ git submodule init
    [tim@thinkpad nodegit2]$ git submodule update 

Then simply run `make unittest` in the project root.

Example of new method:
    [tim@thinkpad Projects]$ cd nodegit2
    [tim@thinkpad nodegit2]$ node-waf configure build
    [tim@thinkpad nodegit2]$ make unittest 

##### Old way #####
nodegit2 utilizes nodeunit `npm install nodeunit` or use `/vendor/nodeunit` to handle its tests in the
`/test` folder.

Example of running repo tests with vendor script:
    [tim@thinkpad Projects]$ cd nodegit2
    [tim@thinkpad nodegit2]$ node-waf configure build
    [tim@thinkpad nodegit2]$ ./vendor/nodeunit/bin/nodeunit test/test-repo.js 

You will most likely install nodeunit via npm or make an alias to the nodeunit binary in `/vendor`.

Release information
-------------------

The release schedule Tim Fontaine and I have decided on (atm) is the following:

### v0.0.1: ###
    * 1:1 mapping of libgit2 read methods
    * An API that can be easily extended with convenience methods in JS

### v0.0.2: ###
    * Write capabilities

### v0.0.3: ###
    * Custom odb backend

Getting involved
----------------

If you like what we are doing and would like to contribute, please fork or leave issues.

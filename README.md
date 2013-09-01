nodegit
=======

> Node.js libgit2 bindings

**v0.0.78** [![Build
Status](https://travis-ci.org/tbranyen/nodegit.png)](https://travis-ci.org/tbranyen/nodegit)

Maintained by Tim Branyen [@tbranyen](http://twitter.com/tbranyen) and Michael
Robinson [@codeofinterest](http://twitter.com/codeofinterest), with help from
[awesome
contributors](https://github.com/tbranyen/nodegit/contributors)!

API Documentation
------------------------

Documentation may be found here: [`nodegit` documentation](http://tbranyen.github.com/nodegit/).

Contributing
------------

Nodegit aims to eventually provide native asynchronous bindings for as much of
libgit2 as possible, but we can't do it alone!

We welcome pull requests, but please pay attention to the following: whether
your lovely code fixes a bug or adds a new feature, please include unit tests
that either prove the bug is fixed, or that your new feature works as expected.
See [running tests](#running-tests)

Unit tests are what makes the Node event loop go around.

Building and installing
-----------------------

### Dependencies ###
To install `nodegit` you need `Node.js`, `python` and `cmake`. To run unit tests you will need to have
`git` installed and accessible from your `PATH` to fetch any `vendor/` addons.

### Easy install (Recommended) ###
This will install and configure everything you need to use `nodegit`.

```` bash
$ npm install nodegit
````

### Mac OS X/Linux/Unix ###

#### Install `nodegit` by cloning source from GitHub and running `node install`: ####

```` bash
# Install system dependencies
$ brew install cmake libzip
$ npm install -g node-gyp
````

```` bash
$ git clone git://github.com/tbranyen/nodegit.git
$ cd nodegit
$ node install
````

\*Updating to a new version\*

```` bash
$ git pull
$ node install
````

### Windows via Cygwin ###

#### `nodegit` has been compiled and tested to work with the setup required to build and run `Node.js` itself. ####

Instructions on compiling `Node.js` on a Windows platform can be found here:
[https://github.com/ry/node/wiki/Building-node.js-on-Cygwin-(Windows)](https://github.com/ry/node/wiki/Building-node.js-on-Cygwin-%28Windows%29)

API Example Usage
-----------------

### Git Log Emulation ###

#### Convenience API ####

```JavaScript
// Load in the module.
var git = require('nodegit'),
    async = require('async');

// Open the repository in the current directory.
git.repo('.git', function(error, repository) {
  if (error) {
    throw error;
  }

  // Use the master branch.
  repository.branch('master', function(error, branch) {
    if (error) {
      throw error;
    }

    // Iterate over the revision history.
    branch.history().on('commit', function(error, commit) {

      // Print out `git log` emulation.
        async.series([
            function(callback) {
                commit.sha(callback);
            },
            function(callback) {
                commit.time(callback);
            },
            function(callback) {
                commit.author(function(error, author) {
                    author.name(callback);
                });
            },
            function(callback) {
                commit.author(function(error, author) {
                    author.email(callback);
                });
            },
            function(callback) {
                commit.message(callback);
            }
        ], function printCommit(error, results) {
            console.log('SHA ' + results[0]);
            console.log(new Date(results[1] * 1000));
            console.log(results[2] + ' <' + results[3] + '>');
            console.log(results[4]);
        });
    });
  });
});
```

Running tests
-------------

__`nodegit` library code is written adhering to a modified `JSHint`. Run these checks with `make lint` in the project root.__

__To run unit tests ensure to update the submodules with `git submodule update --init` and install the development dependencies nodeunit and rimraf with `npm install`.__

Then simply run `npm test` in the project root.

Release information
-------------------

__Can keep track of current method coverage at: [http://bit.ly/tb_methods](http://bit.ly/tb_methods)__

### v0.0.7: ###
    * Updated to work with Node ~0.8.
    * More unit tests
    * Added convenience build script
    * Locked libgit2 to version 0.15.0

### v0.0.6: ###
    * Updated to work with Node ~0.6.

### v0.0.5: ###
    * Added in fast Buffer support.
    * Blob raw write supported added, no convenience methods yet...
    * Updated libgit2 to version 0.12.0

### v0.0.4: ###
    * Many fixes!
    * Blob raw write supported added, no convenience methods yet...
    * Updated libgit2 to version 0.12.0

### v0.0.3: ###
    * More documented native source code
    * Updated convenience api code
    * More unit tests
    * Updated libgit2 to version 0.11.0
    * Windows Cygwin support! *albeit hacky*

### v0.0.2: ###
    * More methods implemented
    * More unit tests
    * More API development
    * Tree and Blob support
    * Updated libgit2 to version 0.8.0

### v0.0.1: ###
    * Some useful methods implemented
    * Some unit tests
    * Some documented source code
    * Useable build/code quality check tools
    * Node.js application that can be configured/built/installed via source and NPM
    * An API that can be easily extended with convenience methods in JS
    * An API that offers a familiar clean syntax that will make adoption and use much more likely
    * Open for public testing
    * GitHub landing page
    * Repo, Oid, Commit, Error, Ref, and RevWalk support
    * Built on libgit2 version 0.3.0

Getting involved
----------------

If you find this project of interest, please document all issues and fork if
you feel you can provide a patch.  Testing is of huge importance; by simply
running the unit tests on your system and reporting issues you can contribute!

__Before submitting a pull request, please ensure both that you've added unit
tests to cover your shiny new code, and that all unit tests and lint checks
pass.__

[![githalytics.com alpha](https://cruel-carlota.pagodabox.com/a81b20d9f61dbcdc7c68002c6a564b5b "githalytics.com")](http://githalytics.com/tbranyen/nodegit)

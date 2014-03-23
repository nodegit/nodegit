nodegit
=======

> Node.js libgit2 bindings

**v0.1.1** [![Build
Status](https://travis-ci.org/nodegit/nodegit.png)](https://travis-ci.org/nodegit/nodegit)

Maintained by Tim Branyen [@tbranyen](http://twitter.com/tbranyen), Michael
Robinson [@codeofinterest](http://twitter.com/codeofinterest), and Nick Kallen [@nk](http://twitter.com/nk), with help from
[awesome
contributors](https://github.com/tbranyen/nodegit/contributors)!

API Documentation
------------------------

Documentation may be found here: [`nodegit` documentation](http://www.nodegit.org/nodegit/).

Building and installing
-----------------------

### Dependencies ###

To install `nodegit` you need `Node.js`, `python` and `cmake` (>=2.8).

### Easy install (Recommended) ###
This will install and configure everything you need to use `nodegit`.

```` bash
$ npm run-script gen && npm install && npm test
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
$ npm run-script gen && npm install
````

### Windows via Cygwin ###

#### `nodegit` has been compiled and tested to work with the setup required to build and run `Node.js` itself. ####

Instructions on compiling `Node.js` on a Windows platform can be found here:
[https://github.com/ry/node/wiki/Building-node.js-on-Cygwin-(Windows)](https://github.com/ry/node/wiki/Building-node.js-on-Cygwin-%28Windows%29)

API Example Usage
-----------------

Below are two examples. [There are several more](https://github.com/nodegit/nodegit/tree/master/example).

### Git Log Emulation ###

```JavaScript
var git = require('../'),
    path = require('path');

git.Repo.open(path.resolve(__dirname, '/tmp/repo/.git'), function(error, repo) {
  if (error) throw error;

  repo.getMaster(function(error, branch) {
    if (error) throw error;

    // History returns an event.
    var history = branch.history();

    // History emits 'commit' event for each commit in the branch's history
    history.on('commit', function(commit) {
      console.log('commit ' + commit.sha());
      console.log('Author:', commit.author().name() + ' <' + commit.author().email() + '>');
      console.log('Date:', commit.date());
      console.log('\n    ' + commit.message());
    });

    // Don't forget to call `start()`!
    history.start();
  });
});

```

### Clone a repo and read a file ###

```JavaScript
git.Repo.clone("https://github.com/nodegit/nodegit.git", path, null, function(error, repo) {
  if (error) throw error;

  repo.getCommit('59b20b8d5c6ff8d09518454d4dd8b7b30f095ab5', function(error, commit) {
    if (error) throw error;

    commit.getEntry('README.md', function(error, entry) {
      if (error) throw error;

      entry.getBlob(function(error, blob) {
        if (error) throw error;

        console.log(entry.name(), entry.sha(), blob.size() + 'b');
        console.log('========================================================\n\n');
        var firstTenLines = blob.toString().split('\n').slice(0, 10).join('\n');
        console.log(firstTenLines);
        console.log('...');
      });
    });
  });
});
```

[![githalytics.com alpha](https://cruel-carlota.pagodabox.com/a81b20d9f61dbcdc7c68002c6a564b5b "githalytics.com")](http://githalytics.com/tbranyen/nodegit)

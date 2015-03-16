NodeGit
-------

> Node bindings to the [libgit2](http://libgit2.github.com/) project.

<table>
  <thead>
    <tr>
      <th>Linux</th>
      <th>OS X</th>
      <th>Windows</th>
      <th>Dependencies</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td colspan="2" align="center">
        <a href="https://travis-ci.org/nodegit/nodegit"><img src="https://travis-ci.org/nodegit/nodegit.svg"></a>
      </td>
      <td align="center">
        <a href="https://ci.appveyor.com/project/timbranyen/nodegit"><img src="https://ci.appveyor.com/api/projects/status/e5a5q75l9yfhnfv2?svg=true"></a>
      </td>
      <td align="center">
        <a href="https://david-dm.org/nodegit/nodegit"><img src="https://david-dm.org/nodegit/nodegit.svg"></a>
      </td>
    </tr>
  </tbody>
</table>

***NOTE: NodeGit currently does not work with [iojs on windows](https://github.com/rvagg/pangyp/issues/4)***

**Stable: 0.3.3**

## Have a problem? Come chat with us! ##

[![Gitter](http://www.nodegit.org/img/gitterbadge.svg)]
(https://gitter.im/nodegit/nodegit)

## Maintained by ##
Tim Branyen [@tbranyen](http://twitter.com/tbranyen),
John Haley [@johnhaley81](http://twitter.com/johnhaley81),
Max Korp [@maxkorp](http://twitter.com/MaximilianoKorp), and
Steve Smith [@orderedlist](https://twitter.com/orderedlist) with help from tons of
[awesome contributors](https://github.com/nodegit/nodegit/contributors)!

### Alumni Maintainers ###
Michael Robinson [@codeofinterest](http://twitter.com/codeofinterest), and
Nick Kallen [@nk](http://twitter.com/nk)

## API Documentation. ##

[http://www.nodegit.org/](http://www.nodegit.org/)

## Getting started. ##

NodeGit will work on most systems out-of-the-box without any native
dependencies.

``` bash
npm install nodegit
```

If you encounter problems while installing, you should try the Building from
source instructions below.

## API examples. ##

### Cloning a repository and reading a file: ###

``` javascript
var clone = require("./").Clone.clone;

// Clone a given repository into a specific folder.
clone("https://github.com/nodegit/nodegit", "tmp", null)
  // Look up this known commit.
  .then(function(repo) {
    // Use a known commit sha from this repository.
    return repo.getCommit("59b20b8d5c6ff8d09518454d4dd8b7b30f095ab5");
  })
  // Look up a specific file within that commit.
  .then(function(commit) {
    return commit.getEntry("README.md");
  })
  // Get the blob contents from the file.
  .then(function(entry) {
    // Patch the blob to contain a reference to the entry.
    return entry.getBlob().then(function(blob) {
      blob.entry = entry;
      return blob;
    });
  })
  // Display information about the blob.
  .then(function(blob) {
    // Show the name, sha, and filesize in byes.
    console.log(blob.entry.name() + blob.entry.sha() + blob.size() + "b");

    // Show a spacer.
    console.log(Array(72).join("=") + "\n\n");

    // Show the entire file.
    console.log(String(blob));
  })
  .catch(function(err) { console.log(err); });

```

### Emulating git log: ###

``` javascript
var open = require("nodegit").Repository.open;

// Open the repository directory.
open("tmp")
  // Open the master branch.
  .then(function(repo) {
    return repo.getMasterCommit();
  })
  // Display information about commits on master.
  .then(function(firstCommitOnMaster) {
    // Create a new history event emitter.
    var history = firstCommitOnMaster.history();

    // Create a counter to only show up to 9 entries.
    var count = 0;

    // Listen for commit events from the history.
    history.on("commit", function(commit) {
      // Disregard commits past 9.
      if (++count >= 9) {
        return;
      }

      // Show the commit sha.
      console.log("commit " + commit.sha());

      // Store the author object.
      var author = commit.author();

      // Display author information.
      console.log("Author:\t" + author.name() + " <", author.email() + ">");

      // Show the commit date.
      console.log("Date:\t" + commit.date());

      // Give some space and show the message.
      console.log("\n    " + commit.message());
    });

    // Start emitting events.
    history.start();
  });
```

For more examples, check the `examples/` folder.

## Unit tests. ##

You will need to build locally before running the tests.  See above.

``` bash
npm test
```

## Migrating from old versions. ##

The bump from 0.1.4 to 0.2.0 was a big one. Many things changed, see here:
https://github.com/nodegit/nodegit/compare/v0.1.4...v0.2.0

This update is wholly and entirely a breaking one, and older versions won't be
maintained. For the purpose of migration, perhaps the biggest point to make
is that async methods can now use promises, rather than just taking callbacks.
Additionally, lots of method and property names have changed.

## nw.js (Node-Webkit) ##

### Native compilation for nw.js ###
A common issue is with NodeGit not functioning properly inside of
[nw.js](http://github.com/nwjs/nw.js) applications. Because NodeGit
is a native module, it has to be rebuilt for node-webkit using
[nw-gyp](http://github.com/rogerwang/nw-gyp). By default, NodeGit will look
in the root package's package.json for an `engines` property, and within look
for a `nw.js` property (or a `node-webkit` if the prior isn't found) that holds
a specific version of nw.js. The value of this property is what will get passed
as the `--target` argument to `nw-gyp configure`.

### Version incompatibility ###
Prior to version 0.2.6, NodeGit used [nan](http://github.com/rvagg/nan) v1.4.3.
As of 0.2.6, NodeGit uses nan v1.5.1 to provide support for io.js. Unfortunately,
this breaks some nw.js compatibility. With nw.js 0.12+, the name was changed to
nw.js from node-webkit. The alpha currently still breaks with NodeGit due to the
nan update, but should be fixed in the final v0.12.0 release. Åpplications using
previous versions of node webkit have 2 options:
1) Use an older version (v0.2.4 or earlier) of NodeGit
2) Use [npm shrinkwrap](https://docs.npmjs.com/cli/shrinkwrap) to force NodeGit to
use nan v1.4.3. Since the binary always recompiles when being used with nw.js, you
shouldn't have to do anything else to make sure it works. As of NodeGit v0.2.6,
the change to nan v1.4.3 doesn't cause any problems.  

Currently, support for nw.js is limited, although we intend to support it better
in the future.

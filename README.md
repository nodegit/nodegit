NodeGit
-------

> Node bindings to the [libgit2](http://libgit2.github.com/) project.

<table>
  <thead>
    <tr>
      <th>Linux</th>
      <th>OS X</th>
      <th>Windows</th>
      <th>Coverage</th>
      <th>Dependencies</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td colspan="2" align="center">
        <a href="https://travis-ci.org/nodegit/nodegit"><img src="https://api.travis-ci.org/nodegit/nodegit.svg?branch=master"></a>
      </td>
      <td align="center">
        <a href="https://ci.appveyor.com/project/timbranyen/nodegit"><img src="https://ci.appveyor.com/api/projects/status/e5a5q75l9yfhnfv2?svg=true"></a>
      </td>
      <td align="center">
        <a href="https://coveralls.io/r/nodegit/nodegit"><img src="https://coveralls.io/repos/nodegit/nodegit/badge.svg" alt="Coverage Status"></a>
      </td>
      <td align="center">
        <a href="https://david-dm.org/nodegit/nodegit"><img src="https://david-dm.org/nodegit/nodegit.svg"></a>
      </td>
    </tr>
  </tbody>
</table>

**Stable (libgit2@v0.26.0): 0.26.0**

## Have a problem? Come chat with us! ##

Visit [slack.libgit2.org](http://slack.libgit2.org/) to sign up, then join us in #nodegit.

## Maintained by ##
Tim Branyen [@tbranyen](http://twitter.com/tbranyen),
John Haley [@johnhaley81](http://twitter.com/johnhaley81), and
Max Korp [@maxkorp](http://twitter.com/MaximilianoKorp) with help from tons of
[awesome contributors](https://github.com/nodegit/nodegit/contributors)!

### Alumni Maintainers ###
Steve Smith [@orderedlist](https://twitter.com/orderedlist),
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

If you receive errors about libstdc++, which are commonly experienced when
building on Travis-CI, you can fix this by upgrading to the latest
libstdc++-4.9.

In Ubuntu:

``` sh
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get update
sudo apt-get install libstdc++-4.9-dev
```

In Travis:

``` yaml
addons:
  apt:
    sources:
      - ubuntu-toolchain-r-test
    packages:
      - libstdc++-4.9-dev
```

In CircleCI:

``` yaml
  dependencies:
    pre:
      - sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
      - sudo apt-get update
      - sudo apt-get install -y libstdc++-4.9-dev
```

If you receive errors about *lifecycleScripts* preinstall/install you probably miss *libssl-dev*
In Ubuntu:
```
sudo apt-get install libssl-dev
```

If you are still encountering problems while installing, you should try the
[Building from source](http://www.nodegit.org/guides/install/from-source/)
instructions.

## API examples. ##

### Cloning a repository and reading a file: ###

``` javascript
var Git = require("nodegit");

// Clone a given repository into the `./tmp` folder.
Git.Clone("https://github.com/nodegit/nodegit", "./tmp")
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
    // Show the path, sha, and filesize in bytes.
    console.log(blob.entry.path() + blob.entry.sha() + blob.rawsize() + "b");

    // Show a spacer.
    console.log(Array(72).join("=") + "\n\n");

    // Show the entire file.
    console.log(String(blob));
  })
  .catch(function(err) { console.log(err); });

```

### Emulating git log: ###

``` javascript
var Git = require("nodegit");

// Open the repository directory.
Git.Repository.open("tmp")
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
      console.log("Author:\t" + author.name() + " <" + author.email() + ">");

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

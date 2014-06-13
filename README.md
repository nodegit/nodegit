NodeGit
-------

> Node bindings to the [libgit2](http://libgit2.github.com/) project.

[![Build
Status](https://travis-ci.org/tbranyen/nodegit.png)](https://travis-ci.org/nodegit/nodegit)
<a href="https://ci.appveyor.com/project/TimBranyen/nodegit"><img src="https://ci.appveyor.com/api/projects/status/e5a5q75l9yfhnfv2/branch/master" alt="Build Status: Windows" height="18" /></a>

**Stable: 0.1.4**

Maintained by Tim Branyen [@tbranyen](http://twitter.com/tbranyen), Michael
Robinson [@codeofinterest](http://twitter.com/codeofinterest), and Nick Kallen
[@nk](http://twitter.com/nk), with help from [awesome
contributors](https://github.com/tbranyen/nodegit/contributors)!

## API Documentation. ##

http://www.nodegit.org/nodegit/

## Getting started. ##

NodeGit will work on most systems out-of-the-box without any native
dependencies.

``` bash
npm install nodegit
```

If you encounter problems while installing, you should try the Building from
source instructions below.

## Building from source. ##

Minimum dependencies:

- [Python 2](https://www.python.org/)

If you wish to help contribute to nodegit it is useful to build locally.

``` bash
# Fetch this project.
git clone git://github.com/tbranyen/nodegit.git

# Enter the repository.
cd nodegit

# Install the template engine, run the code generation script, and install.
npm install ejs && npm run codegen && npm install
```

If you encounter errors, you most likely have not configured the dependencies
correctly.

### Installing dependencies: ###

#### Mac OS X ####

- [Install XCode Command Line Tools](http://railsapps.github.io/xcode-command-line-tools.html)

#### Linux ####

Using APT in Ubuntu:

``` bash
sudo apt-get install build-essential
```

Using Pacman in Arch Linux:

``` bash
sudo pacman -S base-devel
```

#### Windows ####

- [Download and install Python](https://www.python.org/download/windows).
- [Download and install VS Express](http://www.visualstudio.com/downloads/download-visual-studio-vs#d-express-windows-desktop).

You may have to add a build flag to the installation process to successfully
install.  Try first without, if the build fails, try again with the flag.

*Allegedly the order in which you install Visual Studio could trigger this
error.*

``` bash
npm install nodegit --msvs_version=2013
# Or whatever version you've installed.
```

## API examples. ##

### Cloning a repository and reading a file: ###

``` javascript
var clone = require("nodegit").Repo.clone;

// Clone a given repository into a specific folder.
clone("https://github.com/nodegit/nodegit", "tmp", null, function(err, repo) {
  if (err) {
    throw err;
  }

  // Use a known commit sha from this repository.
  var sha = "59b20b8d5c6ff8d09518454d4dd8b7b30f095ab5";

  // Look up this known commit.
  repo.getCommit(sha, function(err, commit) {
    if (err) {
      throw err;
    }

    // Look up a specific file within that commit.
    commit.getEntry("README.md", function(err, entry) {
      if (err) {
        throw err;
      }

      // Get the blob contents from the file.
      entry.getBlob(function(err, blob) {
        if (err) {
          throw err;
        }

        // Show the name, sha, and filesize in byes.
        console.log(entry.name() + entry.sha() + blob.size() + "b");

        // Show a spacer.
        console.log(Array(72).join("=") + "\n\n");

        // Show the entire file.
        console.log(String(blob));
      });
    });
  });
});
```

### Emulating git log: ###

``` javascript
var open = require("nodegit").Repo.open;

// Open the repository directory.
open("tmp", function(err, repo) {
  if (err) {
    throw err;
  }

  // Open the master branch.
  repo.getMaster(function(err, branch) {
    if (err) {
      throw err;
    }

    // Create a new history event emitter.
    var history = branch.history();

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
});
```

## Unit tests. ##

You will need to build locally before running the tests.  See above.

``` bash
npm test
```

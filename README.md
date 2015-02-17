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

**Stable: 0.2.7**

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

http://www.nodegit.org/

## Getting started. ##

NodeGit will work on most systems out-of-the-box without any native
dependencies.

``` bash
npm install nodegit
```

If you encounter problems while installing, you should try the Building from
source instructions below.

## Building from source. ##

If you wish to help contribute to NodeGit it is useful to build locally.

``` bash
# Fetch this project.
git clone git://github.com/nodegit/nodegit.git

# Enter the repository.
cd nodegit

# Install all dependencies, run the code generation scripts, and build.
npm install
```

If you encounter errors, you most likely have not configured the dependencies correctly.

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

Note that GCC/G++ 4.7+ are required, as the library makes use of some c++11 std calls.

#### Windows ####

- [Download and install Python](https://www.python.org/download/windows).
- [Download and install VS Express](http://www.visualstudio.com/downloads/download-visual-studio-vs#d-express-windows-desktop).

You may have to add a build flag to the installation process to successfully install.  
Try first without, if the build fails, try again with the flag.

*Allegedly the order in which you install Visual Studio could trigger this error.*

``` bash
npm install nodegit --msvs_version=2013
# Or whatever version you've installed.
```

##### A note on environment variables in Windows #####
In many of the npm scripts (and examples above), things are run like
`BUILD_ONLY=true npm install`. This sets the `BUILD_ONLY` environment variable
to true for the duration of that command. This doesn't work in windows, however
there is a solution. You can use cmd to call a command inside of cmd (very meta)
with the variable set, and it only lasts for the duration of the inner call to cmd.
So for the above example, you would run `cmd /C "set BUILD_ONLY=true && npm install"`.
See here for more details:
[SuperUser](http://superuser.com/questions/223104/setting-environment-variable-for-just-one-command-in-windows-cmd-exe).

### Debug build: ###

In order to track down possible bugs, you will need a debug buid so you
can get a backtrace with [gdb](http://www.gnu.org/software/gdb/) or
[lldb](http://lldb.llvm.org/).

If you're building for the first time, run `npm run installDebug` (or `BUILD_ONLY=true npm link`)

Note that you should run `rm -rf build/Release` (or `rd /s /q build/Release` in Windows) to make sure a release build doesn't get loaded instead of the debug build.

If you're doing a subsequent rebuild of NodeGit in debug, the clean function will cause a lot of extraneous recompilation of things you probably didn't change (like the vendor dependencies). If you need to regenerate the C++ files and recompile you can run `npm run rebuildDebug`, or `npm run recompileDebug` if you've manually updated the C++ files and don't want them to regenerate.

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

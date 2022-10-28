---
layout: full
menu_item: guides
title: From source
description: How to build NodeGit from source
---

[Return to all guides](../../)

* * *

Building from source
--------------------

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

#### <a name="osx"></a> Mac OS X ####

- [Install XCode Command Line Tools](http://railsapps.github.io/xcode-command-line-tools.html)

#### <a name="linux"></a>Linux ####

Using APT in Ubuntu:

``` bash
sudo apt-get install build-essential
```

Using Pacman in Arch Linux:

``` bash
sudo pacman -S base-devel
```

Note that GCC/G++ 4.7+ are required, as the library makes use of some c++11 std calls.

#### <a name="windows"></a>Windows ####

- [Download and install Python 2](https://www.python.org/download/windows).
- [Download and install VS Community](https://www.visualstudio.com/products/visual-studio-community-vs).

You may have to add a build flag to the installation process to successfully install.
Try first without, if the build fails, try again with the flag.

*Allegedly the order in which you install Visual Studio could trigger this error.*

``` bash
npm install nodegit --msvs_version=2013
# Or whatever version you've installed.
```

### Electron and OpenSSL ###
A local version of OpenSSL is required when building for Electron on Windows and macOS. This is due to Electron using BoringSSL, as we are not able to link to it like we are OpenSSL in Node. Additionally, OpenSSL can be statically linked on Linux by setting the `NODEGIT_OPENSSL_STATIC_LINK` environment variable to `1`.

`acquireOpenSSL.js` will attempt to download OpenSSL prebuilts from S3. If preferred, it can also be built locally by setting the environment variable `npm_config_openssl_bin_url=skip`. On macOS, this should Just Work(tm). On Windows, things are a little trickier.

- We rely on the Visual Studio dev tools to be installed, specifically `vcvarsall.bat` to provide access to the tools. If this is not in the default location for VS2017, you'll need to `npm config set vcvarsall_path <path>` or set the environment variable `npm_config_vcvarsall_path` pointing to it.
- See [Compilation and Installation](https://wiki.openssl.org/index.php/Compilation_and_Installation#Windows) regarding required dependencies, specifically `Perl` (Strawberry Perl is known to work) and `NASM`. Make sure they're on the PATH.

Alternatively, you can provide your own OpenSSL binaries and headers. These can either go in `vendor/openssl` (e.g. `<nodegit_root>/vendor/openssl/{lib,bin,include}` should exist) or in an external directory located by `npm config set openssl_dir <path>` or the environment variable `npm_config_openssl_dir`. Additionally, you can `npm config set openssl_bin_url <url>` or the environment variable `npm_config_openssl_bin_url` to download and extract prebuilt binaries (only supports tar.gz files). `npm config set openssl_bin_sha256 <sha256>` or the environment variable `npm_config_openssl_bin_sha256` can be set to verify the download.

##### A note on environment variables in Windows #####
In many of the npm scripts (and examples above), things are run like
`BUILD_ONLY=true npm install`. This sets the `BUILD_ONLY` environment variable
to true for the duration of that command. This doesn't work in windows, however
there is a solution. You can use cmd to call a command inside of cmd (very meta)
with the variable set, and it only lasts for the duration of the inner call to cmd.
So for the above example, you would run `cmd /C "set BUILD_ONLY=true && npm install"`.
See here for more details:
[SuperUser](http://superuser.com/questions/223104/setting-environment-variable-for-just-one-command-in-windows-cmd-exe).

### <a name="debug-build"></a>Debug build: ###

In order to track down possible bugs, you will need a debug build so you
can get a backtrace with [gdb](http://www.gnu.org/software/gdb/) or
[lldb](http://lldb.llvm.org/).

If you're building for the first time, run `npm run installDebug` (or `BUILD_ONLY=true npm link`)

Note that you should run `rm -rf build/Release` (or `rd /s /q build/Release` in Windows) to make sure a release build doesn't get loaded instead of the debug build.

If you're doing a subsequent rebuild of NodeGit in debug, the clean function will cause a lot of extraneous recompilation of things you probably didn't change (like the vendor dependencies). If you need to regenerate the C++ files and recompile you can run `npm run rebuildDebug`, or `npm run recompileDebug` if you've manually updated the C++ files and don't want them to regenerate.

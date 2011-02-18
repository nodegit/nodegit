NodeJS libgit2 bindings
=======================

Written by Tim Branyen @tbranyen 
with guidance from Tim Fontaine @tjfontaine

Currently under active development, this branch will provide native extension methods to the libgit2 C API.

Building
--------

#### Requirements ####
To use node-libgit2 development tree, you will need to have the `libgit2` api in `/usr/local/lib` and the NodeJS
framework installed.

    * Install libgit2 from [http://libgit2.github.com/](http://libgit2.github.com/) 
    * Install NodeJS from <a href="http://nodejs.org/">http://nodejs.org/</a>

Unit testing
------------

##### New way #####
Ensure the submodules `nodeunit` and `rimraf` are located in the `/vendor` subdirectory.

If they are not, `cd` into the `node-libgit2` dir and run the following git commands:
    [tim@thinkpad Projects]$ cd node-libgit2
    [tim@thinkpad node-libgit2]$ git submodule init
    [tim@thinkpad node-libgit2]$ git submodule update 

Then simply run `make unittest` in the project root.

Example of new method:
    [tim@thinkpad Projects]$ cd node-libgit2
    [tim@thinkpad node-libgit2]$ node-waf configure build
    [tim@thinkpad node-libgit2]$ make unittest 

##### Old way #####
node-libgit2 utilizes nodeunit `npm install nodeunit` or use `/vendor/nodeunit` to handle its tests in the
`/test` folder.

Example of running repo tests with vendor script:
    [tim@thinkpad Projects]$ cd node-libgit2
    [tim@thinkpad node-libgit2]$ node-waf configure build
    [tim@thinkpad node-libgit2]$ ./vendor/nodeunit/bin/nodeunit test/test-repo.js 

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

node.js libgit2 bindings
========================

Written by Tim Branyen @tbranyen 
with guidance from Tim Fontaine @tjfontaine

Currently under active development, this branch will provide native extension methods to the libgit2 C API.

Building development
--------------------

#### Requirements ####
To use node-libgit2 development tree, you will need to have the libgit2 api in /usr/local/lib and the NodeJS
framework installed.

#### Order of importance ####

* Install libgit2 from [http://libgit2.github.com/](http://libgit2.github.com/) 
* Install NodeJS from [http://node.js.org/](http://node.js.org/)
* Install NPM, [http://npmjs.org/](http://npmjs.org/) or install the packages from GitHub source.
* Install nodeutils via `npm install nodeutils` to run the Unit Tests.

Unit testing
------------

node-libgit2 utilizes nodeunit `npm install nodeunit` to handle its tests in the /test folder.

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

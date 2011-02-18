node.js libgit2 bindings
========================

Tim Branyen @tbranyen and Tim Fontaine @tjfontaine

Currently under active development, this branch will provide native extension methods to the libgit2 C API.

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

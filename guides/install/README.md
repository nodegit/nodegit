---
layout: default
menu_item: guides
return_to:
  "All Guides": /guides/
title: Install Guide
description: How to install NodeGit
---

Install
=======

From NPM
--------

To install from NPM you can issue the following command:

``` bash
npm install nodegit
```

From GitHub
-----------

This is required to contribute or run the examples.

Start by cloning the repository, chances are you will only need the latest
commit, so you can pass the `--depth` flag to make a shallow clone:

``` bash
git clone --depth=1 https://github.com/nodegit/nodegit
```

Change your directory into the newly created nodegit folder.

``` bash
cd nodegit
```

Now you can issue the local installation command:

``` bash
npm install
```

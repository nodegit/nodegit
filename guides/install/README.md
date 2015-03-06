---
layout: full
menu_item: guides
title: Install Basics
description: How to install NodeGit
---

[Return to install guides](../)

* * *

<a name="with-npm"></a>From NPM
-------------------------------

To install from the NPM repository you can issue the following command:

``` bash
npm install nodegit
```

<a name="from-github"></a>From GitHub
-------------------------------------

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

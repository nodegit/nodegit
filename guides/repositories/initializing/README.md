---
layout: full
menu_item: guides
title: Initializing
description: How to initialize a repository
---

**In order to run examples, you will need to [Install NodeGit](../../install/basics)
first.**

[Return to repository guides](../)

* * *

Initializing a Repository
-------------------------

This guide explains how to create a new repository.

[View example source](index.js)

### Requiring NodeGit

In the guides directory, we like to keep our NodeGit relative to the project
root.

``` javascript
var NodeGit = require("../../../");
```

However, in your project you will most likely be using the following command:

``` javascript
var NodeGit = require("nodegit");
```

### Arguments to initialize a repo

There are 2 arguments to the `init` method, a path to initialize the repo in
and whether or not to make a `.git` subfolder in that directory or use the
passed in directory as the `.git` folder itself.


``` javascript
var pathToRepo = require("path").resolve("../my-git-projects/my-project");
var isBare = 0; // lets create a .git subfolder
```


### Initialize the Repo

Now that we have our arguments we can call the `init` method on the
`NodeGit.Repository` module to create the repo.

``` javascript
NodeGit.Repository.init(pathToRepo, isBare).then(function (repo) {
  // In this function we have a repo object that we can perform git operations
  // on.

  // Note that with a new repository many functions will fail until there is
  // an initial commit.
});
```

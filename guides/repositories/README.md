---
layout: full
menu_item: guides
title: Opening a Repository
description: How to open and free a repository
---

**In order to run examples, you will need to [Install NodeGit](../../install/basics)
first.**

[Return to all guides](../)

* * *

Opening a Repository
--------------------

This guide explains how to open a repository, and how to work with errors in a
promise chain

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

### Path to Repo

The only argument to the `open` method is a path to the repo on disk. Here we
are calculating that from our current directory using the `path` object from
node.

``` javascript
var pathToRepo = require("path").resolve("../my-git-projects/my-project");
```

You can also point it directly to a `.git` folder to open as well

``` javascript
var pathToRepo = require("path").resolve("../my-git-projects/my-project/.git");
```

This is not necessary though as the function will check the passed directory
for the `.git` subdirectory

### Open a Repo

Now that we have our path to the repo we wish to open we can do so by calling
the `open` method on the `NodeGit.Repository` module

``` javascript
NodeGit.Repository.open(pathToRepo).then(function (repo) {
  // Inside of this function we have an open repo
});
```

*NOTE: We use promises to perform operations in NodeGit. This allows the node event
loop to keep cycling through while under the hood our wrapped libgit2 code is
performing the actions we requested and we're not waiting for it.

This allows our apps to remain responsive and performant. However if you're
not used to promises then this can take some getting used to. If you need
an introduction you can head over to https://www.promisejs.org/ for some
tutorials.*

### Handling errors

Promises will swallow errors if there isn't code to explicitly handle them.
You can do this through any of the following 3 ways.

#### Providing a second function to the `.then` method

You can pass a second function parameter to the `.then` method that will have
the reason why a promise failed in it's first argument.

``` javascript
NodeGit.Repository.open(pathToRepo).then(function (sucessfulResult) {
  // This is the first function of the then which contains the successfully
  // calculated result of the promise
}, function (reasonForFailure) {
  // This is the second function of the then which contains the reason the
  // promise failed
});
```

#### Including a `.catch` in a chain

You can also append a `.catch` to the end of a promise chain which will
receive any promise failure that isn't previously caught

``` javascript
NodeGit.Repository.open(pathToRepo).then(function (sucessfulResult) {
  // This is the first function of the then which contains the successfully
  // calculated result of the promise
})
.catch(function (reasonForFailure) {
  // failure is handled here
});
```

#### Finishing a chain with `done`

If you append a `.done` at the end of your chain, you will have any error that
wasn't previously handled by the above 2 methods thrown.

``` javascript
NodeGit.Repository.open(pathToRepo).then(function (sucessfulResult) {
  // This is the first function of the then which contains the successfully
  // calculated result of the promise
})
.done(function () {
  // If we have a .done then the error will be thrown if there was an error that
  // wasn't caught by either providing a 2nd function to the `.then` or a
  // `.catch` function
});
```

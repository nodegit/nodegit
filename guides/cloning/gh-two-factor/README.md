---
layout: full
menu_item: guides
title: GitHub Two Factor Auth Guide
description: How to clone with GitHub Two Factor Authorization
---

**In order to run examples, you will need to [Install NodeGit](../../install/basics)
first.**

[Return to cloning guides](../)

* * *

GitHub Two Factor Auth
----------------------

This guide explains how to clone a repository, and in the case of failure,
attempt to open the existing path.

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

### GitHub Personal OAuth Token

Before you can clone a repository, you'll need a GitHub OAuth application
token.  You can find more information on generating one here:

[Creating an access token for command-line use](
https://help.github.com/articles/creating-an-access-token-for-command-line-use/
)

Once you have this token you'll assign it to a variable in your project, for
this example, we'll call it `GITHUB_TOKEN`.

``` javascript
var GITHUB_TOKEN = "<GH_TOKEN>";
```

Keep this variable a secret.  If you accidentally commit this key to a public
GitHub repository they will immediately revoke it.

### Clone URL

The first argument to the `clone` method is a URL.

In this example we're going to clone one of our private test repositories from
GitHub.  This must be an `https` protocol URL for the clone to work.

``` javascript
var cloneURL = "https://github.com/nodegit/private";
```

### Clone path

The second argument to the `clone` method is a path.

Ideally your application will clone a repository into the same folder path
regardless of how or where you execute it from.  Paths are relative to the
current working directory in NodeGit, so you will need to normalize it first.

This is very simple in Node:

``` javascript
var localPath = require("path").join(__dirname, "tmp");
```

Now this `tmp` directory will be created along side your script, no matter how
or where you execute it from.

### Clone options

The third argument to the `clone` method is an optional simple object.

``` javascript
var cloneOptions = {};
```

#### GitHub certificate issue in OS X

Unfortunately in OS X there is a problem where libgit2 is unable to look up
GitHub certificates correctly.  In order to bypass this problem, we're going
to passthrough the certificate check.

*Note: this is not a problem with Windows or Linux*

``` javascript
cloneOptions.remoteCallbacks = {
  certificateCheck: function() { return 1; }
};
```

#### GitHub credentials for Two Factor Auth

In order to authorize the clone operation, we'll need to respond to a low-level
callback that expects credentials to be passed.

This function will be attached below the above `certificateCheck`, and will
respond back with the OAuth token.

The `remoteCallbacks` object now looks like this:

``` javascript
cloneOptions.remoteCallbacks = {
  certificateCheck: function() { return 1; },
  credentials: function() {
    return NodeGit.Cred.userpassPlaintextNew(GITHUB_TOKEN, "x-oauth-basic");
  }
};
```

### Invoking the clone method

You can easily invoke our top-level Clone as a function passing along the three
aforementioned arguments.

``` javascript
var cloneRepository = NodeGit.Clone(cloneURL, localPath, cloneOptions);
```

Notice how we store the return value from `Git.Clone`.  This is a
[Promise](https://www.promisejs.org/) to represent the asynchronous operation.
It offers finer control flow by allowing us to capture errors and fallback if
there is a clone failure.

### Handling clone failure

A naive way to handle a clone failure is to try opening the same path.  Clones
will most commonly fail when the directory already exists.  We can define
a function to attempt opening in this case.

``` javascript
var errorAndAttemptOpen = function() {
  return NodeGit.Repository.open(local);
};
```

This will be called as part of the Promise resolution in the final step.

### The Promise chain

Lastly in our clone operation, we'll assemble a Promise chain to handle errors
and work with the `Git.Repository` instance result.

``` javascript
cloneRepository.catch(errorAndAttemptOpen)
  .then(function(repository) {
    // Access any repository methods here.
    console.log("Is the repository bare? %s", Boolean(repository.isBare()));
  });
```

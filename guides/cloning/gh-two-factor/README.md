Cloning
=======

**In order to run examples, you will need to [Install NodeGit](../../install)
first.**

[Return to cloning examples](../)

HTTP/HTTPS clone
----------------

This guide explains how to clone a repository, and in the case of failure,
attempt to open the existing path.

[View example source](index.js)

### Requiring NodeGit

In the guides directory, we like to keep our NodeGit relative to the project
root.

``` javascript
var Git = require("../../../");
```

However, in your project you will most likely be using the following command:

``` javascript
var Git = require("nodegit");
```

### Clone URL

The first argument to the `clone` method is a URL.

In this example we're going to clone one of our test repositories from GitHub.
You could easily substitute this with any valid http or https Git repository
URL.

``` javascript
var cloneURL = "https://github.com/nodegit/test";
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
to bypass the certificate check.

*Note: this is not a problem with Windows or Linux*

``` javascript
cloneOptions.remoteCallbacks = {
  certificateCheck: function() { return 1; }
};
```

### Invoking the clone method

The way NodeGit is structured is that all [libgit2](http://libgit2.org) C
methods are dynamically generated into C++.  Since we're taking a
class-oriented approach, we make a top level class named `Clone`.  This class
has a static method `clone` that we can use to bring down a repository.

While it may look a bit verbose, it is symptomatic of a rigid convention.

``` javascript
var cloneRepository = Git.Clone(cloneURL, localPath, cloneOptions);
```

Notice how we store the return value from `Clone.clone`.  This is a [Promise]()
to represent the asynchronous operation.  It offers finer control flow by
allowing us to capture errors and fallback if there is a clone failure.

### Handling clone failure

A naive way to handle a clone failure is to try opening the same path.  Clones
will most commonly fail when the directory already exists.  We can define
a function to attempt opening in this case.

``` javascript
var errorAndAttemptOpen = function() {
  return Git.Repository.open(local);
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


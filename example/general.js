var git = require('../'),
    path = require('path');

// **nodegit** is a javascript library for node.js that wraps libgit2, a
// pure C implementation of the Git core. It provides an asynchronous
// interface around any functions that do I/O, and a sychronous interface
// around the rest.
//
// This file is an example of using that API in a real, JS file.
//
// **libgit2** (for the most part) only implements the core plumbing
// functions, not really the higher level porcelain stuff. For a primer on
// Git Internals that you will need to know to work with Git at this level,
// check out [Chapter 9][pg] of the Pro Git book.

// Nearly, all git operations in the context of a repository.
// To open a repository,

git.Repo.open(path.resolve(__dirname, '../.git'), function(error, repo) {
  // For all of the following examples, error-handling will be performed in
  // this naive way:  
  if (error) throw error;
  console.log("Opened repository.");

  // ### SHA-1 Value Conversions

  // Objects in git (commits, blobs, etc.) are referred to by their SHA value
  // **nodegit** uses a simple wrapper around hash values called an `Oid`.
  // The oid validates that the SHA is well-formed.

  var oid = git.Oid.fromString('c27d9c35e3715539d941254f2ce57042b978c49c');

  // Most functions in in **nodegit** that take an oid will also take a
  // string, so for example, you can look up a commit by a string SHA or
  // an Oid, but but any functions that create new SHAs will always return
  // an Oid.

  // If you have a oid, you can easily get the hex value of the SHA again.
  console.log("Sha hex string:", oid.sha());

  // ### Working with the Object Database

  // **libgit2** provides [direct access][odb] to the object database. The
  // object database is where the actual objects are stored in Git. For
  // working with raw objects, we'll need to get this structure from the
  // repository.
  var odb = repo.odb();

  // We can read raw objects directly from the object database if we have
  // the oid (SHA) of the object. This allows us to access objects without
  // knowing thier type and inspect the raw bytes unparsed.

  odb.read(oid, function(error, object) {
    if (error) throw error;

    // A raw object only has three properties - the type (commit, blob, tree
    // or tag), the size of the raw data and the raw, unparsed data itself.
    // For a commit or tag, that raw data is human readable plain ASCII
    // text. For a blob it is just file contents, so it could be text or
    // binary data. For a tree it is a special binary format, so it's unlikely
    // to be hugely helpful as a raw object.
    var data = object.data(),
        type = object.type();

    console.log("Object size and type:", object.size(), object.type());
  });

  // You can also write raw object data to Git. This is pretty cool because
  // it gives you direct access to the key/value properties of Git. Here
  // we'll write a new blob object that just contains a simple string.
  // Notice that we have to specify the object type.
  odb.write("test data", "test data".length, git.Object.Type.Blob, function(error, oid) {
    if (error) throw error;

    // Now that we've written the object, we can check out what SHA1 was
    // generated when the object was written to our database.
    console.log("Written Object: ", oid.sha());
  });

  // ### Object Parsing

  // libgit2 has methods to parse every object type in Git so you don't have
  // to work directly with the raw data. This is much faster and simpler
  // than trying to deal with the raw data yourself.

  // #### Commit Parsing

  // [Parsing commit objects][pco] is simple and gives you access to all the
  // data in the commit - the author (name, email, datetime), committer
  // (same), tree, message, encoding and parent(s).

  oid = git.Oid.fromString("698c74e817243efe441a5d1f3cbaf3998282ca86");

  // Many methods in **nodegit** are asynchronous, because they do file
  // or network I/O. By convention, all asynchronous methods are named
  // imperatively, like `getCommit`, `open`, `read`, `write`, etc., whereas
  // synchronous methods are named nominatively, like `type`, `size`, `name`.

  repo.getCommit(oid, function(error, commit) {
    if (error) throw error;

    // Each of the properties of the commit object are accessible via methods,
    // including commonly needed variations, such as `git_commit_time` which
    // returns the author time and `git_commit_message` which gives you the
    // commit message.
    console.log("Commit:", commit.message(), commit.author().name(), commit.date());

    // Commits can have zero or more parents. The first (root) commit will
    // have no parents, most commits will have one (i.e. the commit it was
    // based on) and merge commits will have two or more. Commits can
    // technically have any number, though it's rare to have more than two.
    commit.getParents(function(error, parents) {
      parents.forEach(function(parent) {
        console.log("Parent:", parent.oid().sha());
      });
    });
  });

  // #### Writing Commits

  // nodegit provides a couple of methods to create commit objects easily as
  // well.

  var author = git.Signature.create("Scott Chacon", "schacon@gmail.com", 123456789, 60);
  var committer = git.Signature.create("Scott A Chacon", "scott@github.com", 987654321, 90);

  // Commit objects need a tree to point to and optionally one or more
  // parents. Here we're creating oid objects to create the commit with,
  // but you can also use existing ones:

  var treeId = git.Oid.fromString("4170d10f19600b9cb086504e8e05fe7d863358a2");
  var parentId = git.Oid.fromString("eebd0ead15d62eaf0ba276da53af43bbc3ce43ab");

  repo.getTree(treeId, function(error, tree) {
    if (error) throw error;

    repo.getCommit(parentId, function(error, parent) {
      if (error) throw error;
      // Here we actually create the commit object with a single call with all
      // the values we need to create the commit. The SHA key is written to the
      // `commit_id` variable here.
      repo.createCommit(
        null /* do not update the HEAD */,
        author,
        committer,
        "example commit",
        tree,
        [parent],
        function (error, oid) {
          console.log("New Commit:", oid.sha());
        });
    });
  });

  // #### Tag Parsing

  // You can parse and create tags with the [tag management API][tm], which
  // functions very similarly to the commit lookup, parsing and creation
  // methods, since the objects themselves are very similar.

  oid = git.Oid.fromString("43f0ac7359e30b769f6b1714e0adbaf51bedbb65");
  repo.getTag(oid, function(error, tag) {
    if (error) throw error;

    // Now that we have the tag object, we can extract the information it
    // generally contains: the target (usually a commit object), the type of
    // the target object (usually 'commit'), the name ('v1.0'), the tagger (a
    // git_signature - name, email, timestamp), and the tag message.
    console.log(tag.name(), tag.targetType(), tag.message());

    tag.getTarget(function (error, target) {
      if (error) throw error;

      console.log("Target is commit:", target.isCommit());
    });
  });


  // #### Tree Parsing

  // A Tree is how Git represents the state of the filesystem
  // at a given revision. In general, a tree corresponds to a directory,
  // and files in that directory are either files (blobs) or directories.

  // [Tree parsing][tp] is a bit different than the other objects, in that
  // we have a subtype which is the tree entry. This is not an actual
  // object type in Git, but a useful structure for parsing and traversing
  // tree entries.

  oid = git.Oid.fromString("e1b0c7ea57bfc5e30ec279402a98168a27838ac9");
  repo.getTree(oid, function(error, tree) {
    if (error) throw error;

    console.log("Tree Size:", tree.size());
    function dfs(error, tree) {
      tree.entries().forEach(function(entry) {
        if (entry.isDirectory()) {
          entry.getTree(dfs);
        } else if (entry.isFile()) {
          console.log("Tree Entry:", entry.name());
        }
      });
    }
    dfs(null, tree);

    // You can also access tree entries by path if you know the path of the
    // entry you're looking for.
    tree.getEntry("example/general.js", function(error, entry) {
      if (error) throw error;

      // Entries which are files have blobs associated with them:
      entry.getBlob(function(error, blob) {
        console.log("Blob size:", blob.size());
      });
    });
  });

  // #### Blob Parsing

  // The last object type is the simplest and requires the least parsing
  // help. Blobs are just file contents and can contain anything, there is
  // no structure to it. The main advantage to using the [simple blob
  // api][ba] is that when you're creating blobs you don't have to calculate
  // the size of the content. There is also a helper for reading a file
  // from disk and writing it to the db and getting the oid back so you
  // don't have to do all those steps yourself.

  oid = git.Oid.fromString("991c06b7b1ec6f939488427e4b41a4fa3e1edd5f");
  repo.getBlob(oid, function(error, blob) {
    if (error) throw error;

    // You can access a node.js Buffer with the raw contents of the blob directly.
    // Note that this buffer may not be contain ASCII data for certain blobs
    // (e.g. binary files).
    var buffer = blob.content();

    // If you know that the blob is UTF-8, however, 
    console.log("Blob contents:", blob.toString().slice(0, 38));
  });

  // ### Revwalking

  // The libgit2 [revision walking api][rw] provides methods to traverse the
  // directed graph created by the parent pointers of the commit objects.
  // Since all commits point back to the commit that came directly before
  // them, you can walk this parentage as a graph and find all the commits
  // that were ancestors of (reachable from) a given starting point. This
  // can allow you to create `git log` type functionality.

  oid = git.Oid.fromString("698c74e817243efe441a5d1f3cbaf3998282ca86");

  // To use the revwalker, create a new walker, tell it how you want to sort
  // the output and then push one or more starting points onto the walker.
  // If you want to emulate the output of `git log` you would push the SHA
  // of the commit that HEAD points to into the walker and then start
  // traversing them. You can also 'hide' commits that you want to stop at
  // or not see any of their ancestors. So if you want to emulate `git log
  // branch1..branch2`, you would push the oid of `branch2` and hide the oid
  // of `branch1`.
  var revWalk = repo.createRevWalk();
  revWalk.sorting(git.RevWalk.Sort.Topological, git.RevWalk.Sort.Reverse);
  revWalk.push(oid, function(error) {
    if (error) throw error;

    // Now that we have the starting point pushed onto the walker, we start
    // asking for ancestors. It will return them in the sorting order we asked
    // for as commit oids. We can then lookup and parse the commited pointed
    // at by the returned OID; note that this operation is specially fast
    // since the raw contents of the commit object will be cached in memory

    function walk() {
      revWalk.next(function(error, oid) {
        if (error) throw error;
        if (!oid) return;

        repo.getCommit(oid, function(error, commit) {
          if (error) throw error;

          console.log("Commit:", commit.sha());
          walk();
        });
      });
    }
    walk();
  });

  // ### Index File Manipulation

  // The [index file API][gi] allows you to read, traverse, update and write
  // the Git index file (sometimes thought of as the staging area).
  repo.openIndex(function(error, index) {
    if (error) throw error;

    // For each entry in the index, you can get a bunch of information
    // including the SHA (oid), path and mode which map to the tree objects
    // that are written out. It also has filesystem properties to help
    // determine what to inspect for changes (ctime, mtime, dev, ino, uid,
    // gid, file_size and flags) All these properties are exported publicly in
    // the `IndexEntry` class

    index.entries().forEach(function(entry) {
      console.log("Index Entry:", entry.path(), entry.mtime().seconds());
    });
  });

  // ### References

  // The [reference API][ref] allows you to list, resolve, create and update
  // references such as branches, tags and remote references (everything in
  // the .git/refs directory).

  repo.getReferences(git.Reference.Type.All, function(error, referenceNames) {
    if (error) throw error;

    referenceNames.forEach(function(referenceName) {
      repo.getReference(referenceName, function(error, reference) {
        if (error) throw error;

        if (reference.isConcrete()) {
          console.log("Reference:", referenceName, reference.target());
        } else if (reference.isSymbolic()) {
          console.log("Reference:", referenceName, reference.symbolicTarget());
        }
      });
    });
  });
});



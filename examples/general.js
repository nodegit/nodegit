var nodegit = require("../");
var path = require("path");
var oid;
var odb;
var repo;

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

nodegit.Repository.open(path.resolve(__dirname, "../.git"))
  .then(function(repoResult) {
    repo = repoResult;
    console.log("Opened repository.");

    // ### SHA-1 Value Conversions

    // Objects in git (commits, blobs, etc.) are referred to by their SHA value
    // **nodegit** uses a simple wrapper around hash values called an `Oid`.
    // The oid validates that the SHA is well-formed.

    oid = nodegit.Oid.fromString("c27d9c35e3715539d941254f2ce57042b978c49c");

    // Most functions in in **nodegit** that take an oid will also take a
    // string, so for example, you can look up a commit by a string SHA or
    // an Oid, but but any functions that create new SHAs will always return
    // an Oid.

    // If you have a oid, you can easily get the hex value of the SHA again.
    console.log("Sha hex string:", oid.toString());

    // ### Working with the Object Database

    // **libgit2** provides [direct access][odb] to the object database. The
    // object database is where the actual objects are stored in Git. For
    // working with raw objects, we'll need to get this structure from the
    // repository.
    return repo.odb();
  })

  .then(function(odbResult) {
    odb = odbResult;

    // We can read raw objects directly from the object database if we have
    // the oid (SHA) of the object. This allows us to access objects without
    // knowing thier type and inspect the raw bytes unparsed.

    return odb.read(oid);
  })

  .then(function(object) {
    // A raw object only has three properties - the type (commit, blob, tree
    // or tag), the size of the raw data and the raw, unparsed data itself.
    // For a commit or tag, that raw data is human readable plain ASCII
    // text. For a blob it is just file contents, so it could be text or
    // binary data. For a tree it is a special binary format, so it's unlikely
    // to be hugely helpful as a raw object.
    var data = object.data();
    var type = object.type();
    var size = object.size();

    console.log("Object size and type:", size, type);
    console.log("Raw data: ", data.toString().substring(100), "...");

  })

  .then(function() {
    // You can also write raw object data to Git. This is pretty cool because
    // it gives you direct access to the key/value properties of Git. Here
    // we'll write a new blob object that just contains a simple string.
    // Notice that we have to specify the object type.
    return odb.write("test data", "test data".length, nodegit.Object.TYPE.BLOB);
  })

  .then(function(oid) {
    // Now that we've written the object, we can check out what SHA1 was
    // generated when the object was written to our database.
    console.log("Written Object: ", oid.toString());
  })

  .then(function() {
    // ### Object Parsing

    // libgit2 has methods to parse every object type in Git so you don't have
    // to work directly with the raw data. This is much faster and simpler
    // than trying to deal with the raw data yourself.

    // #### Commit Parsing

    // [Parsing commit objects][pco] is simple and gives you access to all the
    // data in the commit - the author (name, email, datetime), committer
    // (same), tree, message, encoding and parent(s).

    oid = nodegit.Oid.fromString("698c74e817243efe441a5d1f3cbaf3998282ca86");

    // Many methods in **nodegit** are asynchronous, because they do file
    // or network I/O. By convention, all asynchronous methods are named
    // imperatively, like `getCommit`, `open`, `read`, `write`, etc., whereas
    // synchronous methods are named nominatively, like `type`, `size`, `name`.

    return repo.getCommit(oid);
  })

  .then(function(commit) {
    // Each of the properties of the commit object are accessible via methods,
    // including commonly needed variations, such as `git_commit_time` which
    // returns the author time and `git_commit_message` which gives you the
    // commit message.
    console.log("Commit:", commit.message(),
      commit.author().name(), commit.date());

    // Commits can have zero or more parents. The first (root) commit will
    // have no parents, most commits will have one (i.e. the commit it was
    // based on) and merge commits will have two or more. Commits can
    // technically have any number, though it's rare to have more than two.
    return commit.getParents();
  })

  .then(function(parents) {
    parents.forEach(function(parent) {
      console.log("Parent:", parent.toString());
    });
  })

  .then(function() {
    // #### Writing Commits

    // nodegit provides a couple of methods to create commit objects easily as
    // well.
    var author = nodegit.Signature.now("Scott Chacon",
      "schacon@gmail.com");
    var committer = nodegit.Signature.now("Scott A Chacon",
      "scott@github.com");

    // Commit objects need a tree to point to and optionally one or more
    // parents. Here we're creating oid objects to create the commit with,
    // but you can also use existing ones:
    var treeId = nodegit.Oid.fromString(
      "4170d10f19600b9cb086504e8e05fe7d863358a2");
    var parentId = nodegit.Oid.fromString(
      "eebd0ead15d62eaf0ba276da53af43bbc3ce43ab");

    return repo.getTree(treeId).then(function(tree) {
      return repo.getCommit(parentId).then(function(parent) {
        // Here we actually create the commit object with a single call with all
        // the values we need to create the commit. The SHA key is written to
        // the `commit_id` variable here.
        return repo.createCommit(
          null /* do not update the HEAD */,
          author,
          committer,
          "example commit",
          tree,
          [parent]);
      }).then(function(oid) {
        console.log("New Commit:", oid.toString());
      });
    });
  })

  .then(function() {
    // #### Tag Parsing

    // You can parse and create tags with the [tag management API][tm], which
    // functions very similarly to the commit lookup, parsing and creation
    // methods, since the objects themselves are very similar.

    oid = nodegit.Oid.fromString("dcc4aa9fcdaced037434cb149ed3b6eab4d0709d");
    return repo.getTag(oid);
  })

  .then(function(tag) {
    // Now that we have the tag object, we can extract the information it
    // generally contains: the target (usually a commit object), the type of
    // the target object (usually "commit"), the name ("v1.0"), the tagger (a
    // git_signature - name, email, timestamp), and the tag message.
    console.log(tag.name(), tag.targetType(), tag.message());

    return tag.target();
  })

  .then(function (target) {
    console.log("Target is commit:", target.isCommit());
  })

  .then(function() {
    // #### Tree Parsing

    // A Tree is how Git represents the state of the filesystem
    // at a given revision. In general, a tree corresponds to a directory,
    // and files in that directory are either files (blobs) or directories.

    // [Tree parsing][tp] is a bit different than the other objects, in that
    // we have a subtype which is the tree entry. This is not an actual
    // object type in Git, but a useful structure for parsing and traversing
    // tree entries.

    oid = nodegit.Oid.fromString("e1b0c7ea57bfc5e30ec279402a98168a27838ac9");
    return repo.getTree(oid);
  })

  .then(function(tree) {
    console.log("Tree Size:", tree.entryCount());

    function dfs(tree) {
      var promises = [];

      tree.entries().forEach(function(entry) {
        if (entry.isDirectory()) {
          promises.push(entry.getTree().then(dfs));
        } else if (entry.isFile()) {
          console.log("Tree Entry:", entry.name());
        }
      });

      return Promise.all(promises);
    }

    return dfs(tree).then(function() {
      // You can also access tree entries by path if you know the path of the
      // entry you're looking for.
      return tree.getEntry("example/general.js").then(function(entry) {
        // Entries which are files have blobs associated with them:
        entry.getBlob(function(error, blob) {
          console.log("Blob size:", blob.size());
        });
      });
    });
  })

  .then(function() {
    // #### Blob Parsing

    // The last object type is the simplest and requires the least parsing
    // help. Blobs are just file contents and can contain anything, there is
    // no structure to it. The main advantage to using the [simple blob
    // api][ba] is that when you're creating blobs you don't have to calculate
    // the size of the content. There is also a helper for reading a file
    // from disk and writing it to the db and getting the oid back so you
    // don't have to do all those steps yourself.

    oid = nodegit.Oid.fromString("991c06b7b1ec6f939488427e4b41a4fa3e1edd5f");
    return repo.getBlob(oid);
  })

  .then(function(blob) {
    // You can access a node.js Buffer with the raw contents
    // of the blob directly. Note that this buffer may not
    // contain ASCII data for certain blobs (e.g. binary files).
    var buffer = blob.content();

    // If you know that the blob is UTF-8, however,
    console.log("Blob contents:", blob.toString().slice(0, 38));
    console.log("Buffer:", buffer.toString().substring(100), "...");
  })

  .then(function() {
    // ### Revwalking

    // The libgit2 [revision walking api][rw] provides methods to traverse the
    // directed graph created by the parent pointers of the commit objects.
    // Since all commits point back to the commit that came directly before
    // them, you can walk this parentage as a graph and find all the commits
    // that were ancestors of (reachable from) a given starting point. This
    // can allow you to create `git log` type functionality.

    oid = nodegit.Oid.fromString("698c74e817243efe441a5d1f3cbaf3998282ca86");

    // To use the revwalker, create a new walker, tell it how you want to sort
    // the output and then push one or more starting points onto the walker.
    // If you want to emulate the output of `git log` you would push the SHA
    // of the commit that HEAD points to into the walker and then start
    // traversing them. You can also "hide" commits that you want to stop at
    // or not see any of their ancestors. So if you want to emulate `git log
    // branch1..branch2`, you would push the oid of `branch2` and hide the oid
    // of `branch1`.
    var revWalk = repo.createRevWalk();

    revWalk.sorting(nodegit.Revwalk.SORT.TOPOLOGICAL,
      nodegit.Revwalk.SORT.REVERSE);

    revWalk.push(oid);

    // Now that we have the starting point pushed onto the walker, we start
    // asking for ancestors. It will return them in the sorting order we asked
    // for as commit oids. We can then lookup and parse the commited pointed
    // at by the returned OID; note that this operation is specially fast
    // since the raw contents of the commit object will be cached in memory

    function walk() {
      return revWalk.next().then(function(oid) {
        if (!oid) {
          return;
        }

        return repo.getCommit(oid).then(function(commit) {
          console.log("Commit:", commit.toString());
          return walk();
        });
      });
    }

    return walk();
  })

  .then(function() {
    // ### Index File Manipulation

    // The [index file API][gi] allows you to read, traverse, update and write
    // the Git index file (sometimes thought of as the staging area).
    return repo.refreshIndex();
  })

  .then(function(index) {
    // For each entry in the index, you can get a bunch of information
    // including the SHA (oid), path and mode which map to the tree objects
    // that are written out. It also has filesystem properties to help
    // determine what to inspect for changes (ctime, mtime, dev, ino, uid,
    // gid, file_size and flags) All these properties are exported publicly in
    // the `IndexEntry` class

    index.entries().forEach(function(entry) {
      console.log("Index Entry:", entry.path(), entry.mtime().seconds());
    });
  })

  .then(function() {
    // ### References

    // The [reference API][ref] allows you to list, resolve, create and update
    // references such as branches, tags and remote references (everything in
    // the .git/refs directory).

    return repo.getReferenceNames(nodegit.Reference.TYPE.LISTALL);
  })

  .then(function(referenceNames) {
    var promises = [];

    referenceNames.forEach(function(referenceName) {
      promises.push(repo.getReference(referenceName).then(function(reference) {
        if (reference.isConcrete()) {
          console.log("Reference:", referenceName, reference.target());
        } else if (reference.isSymbolic()) {
          console.log("Reference:", referenceName, reference.symbolicTarget());
        }
      }));
    });

    return Promise.all(promises);
  })

  .done(function() {
    console.log("Done!");
  });

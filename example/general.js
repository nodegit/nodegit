var git = require('nodegit');

git.Repo.open('/opt/libgit2-test/.git', function(error, repo) {
  if (error) throw error;

  // ### SHA-1 Value Conversions

  // The `git_oid` is the structure that keeps the SHA value. We will use
  // this throughout the example for storing the value of the current SHA
  // key we're working with.
  var oid = git.Oid.fromString('fd6e612585290339ea8bf39c692a7ff6a29cb7c3');

  // If you have a oid, you can easily get the hex value of the SHA as well.
  console.log(oid.sha());

  // ### Working with the Object Database

  // **libgit2** provides [direct access][odb] to the object database.  The
  // object database is where the actual objects are stored in Git. For
  // working with raw objects, we'll need to get this structure from the
  // repository.
  var odb = repo.odb();

  // We can read raw objects directly from the object database if we have
  // the oid (SHA) of the object.  This allows us to access objects without
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

    console.log(object.size(), object.type());
  });

  // You can also write raw object data to Git. This is pretty cool because
  // it gives you direct access to the key/value properties of Git.  Here
  // we'll write a new blob object that just contains a simple string.
  // Notice that we have to specify the object type as the `git_otype` enum.
  odb.write("test data", git.Object.Type.Blob, function(error, oid) {
    if (error) throw error;

    // Now that we've written the object, we can check out what SHA1 was
    // generated when the object was written to our database.
    console.log(oid.sha());
  });

  // ### Object Parsing

  // libgit2 has methods to parse every object type in Git so you don't have
  // to work directly with the raw data. This is much faster and simpler
  // than trying to deal with the raw data yourself.

  // #### Commit Parsing

  // [Parsing commit objects][pco] is simple and gives you access to all the
  // data in the commit - the author (name, email, datetime), committer
  // (same), tree, message, encoding and parent(s).

  oid = git.Oid.fromString("f0877d0b841d75172ec404fc9370173dfffc20d1");
  repo.getCommit(oid, function(error, commit) {
    if (error) throw error;

    // Each of the properties of the commit object are accessible via methods,
    // including commonly needed variations, such as `git_commit_time` which
    // returns the author time and `git_commit_message` which gives you the
    // commit message (as a NUL-terminated string).
    console.log(commit.message(), commit.author(), commit.committer(), commit.time());

    // Commits can have zero or more parents. The first (root) commit will
    // have no parents, most commits will have one (i.e. the commit it was
    // based on) and merge commits will have two or more.  Commits can
    // technically have any number, though it's rare to have more than two.
    commit.getParents(function(error, parents) {
      parents.forEach(function(parent) {
        console.log(parent.oid());
      });
    });
  });

  // #### Writing Commits

  // libgit2 provides a couple of methods to create commit objects easily as
  // well. There are four different create signatures, we'll just show one
  // of them here.  You can read about the other ones in the [commit API
  // docs][cd].
  //
  // [cd]: http://libgit2.github.com/libgit2/#HEAD/group/commit

  var author = new git.Signature("Scott Chacon", "schacon@gmail.com", 123456789, 60);
  var committer = new git.Signature("Scott A Chacon", "scott@github.com", 987654321, 90);

  // Commit objects need a tree to point to and optionally one or more
  // parents.  Here we're creating oid objects to create the commit with,
  // but you can also use existing ones:
  var treeId = git.Oid.fromString("28873d96b4e8f4e33ea30f4c682fd325f7ba56ac");
  var parentId = git.Oid.fromString("f0877d0b841d75172ec404fc9370173dfffc20d1");

  repo.getTree(treeId, function(error, tree) {
    repo.getCommit(parentId, function(error, parent) {

      // Here we actually create the commit object with a single call with all
      // the values we need to create the commit.  The SHA key is written to the
      // `commit_id` variable here.
      repo.createCommit(
        null /* do not update the HEAD */,
        author,
        committer,
        null /* use default message encoding */,
        "example commit",
        tree,
        1, parent,
        function (error, commitOid) {
          console.log(commitOid.sha());
        });
    });
  });

  // #### Tag Parsing

  // You can parse and create tags with the [tag management API][tm], which
  // functions very similarly to the commit lookup, parsing and creation
  // methods, since the objects themselves are very similar.

  var oid = git.Oid.fromString("bc422d45275aca289c51d79830b45cecebff7c3a");
  repo.getTag(oid, function(error, tag) {
    if (error) throw error;

    // Now that we have the tag object, we can extract the information it
    // generally contains: the target (usually a commit object), the type of
    // the target object (usually 'commit'), the name ('v1.0'), the tagger (a
    // git_signature - name, email, timestamp), and the tag message.
    console.log(tag.name(), tag.type(), tag.message());

    tag.getTarget(function (error, commit) {
      if (error) throw error;
      console.log(commit);
    });
  });

  // #### Tree Parsing

  // [Tree parsing][tp] is a bit different than the other objects, in that
  // we have a subtype which is the tree entry.  This is not an actual
  // object type in Git, but a useful structure for parsing and traversing
  // tree entries.

  var oid = git.Oid.fromString("2a741c18ac5ff082a7caaec6e74db3075a1906b5");
  repo.getTree(oid, function(error, tree) {
    if (error) throw error;

    console.log(tree.size());
    tree.entries().forEach(function(entry) {
      console.log(entry.name());

      if (entry.isDirectory()) {
        entry.getTree(function(error, tree) {
          if (error) throw error;

          console.log("Recursively got tree");
        });
      } else {
        entry.getBlob(function(error, blob) {
          console.log(blob.toString());
        });
      }
    });

    // You can also access tree entries by path if you know the path of the
    // entry you're looking for.
    tree.getFile("/src/hello.c", function(error, entry) {
      entry.getBlob(function(error, blob) {
        console.log(blob.toString());
      });
    });
  });

  // #### Blob Parsing

  // The last object type is the simplest and requires the least parsing
  // help. Blobs are just file contents and can contain anything, there is
  // no structure to it. The main advantage to using the [simple blob
  // api][ba] is that when you're creating blobs you don't have to calculate
  // the size of the content.  There is also a helper for reading a file
  // from disk and writing it to the db and getting the oid back so you
  // don't have to do all those steps yourself.

  var oid = git.Oid.fromString("af7574ea73f7b166f869ef1a39be126d9a186ae0");
  repo.getBlob(oid, function(error, blob) {
    if (error) throw error;

    // You can access a buffer with the raw contents of the blob directly.
    // Note that this buffer may not be contain ASCII data for certain blobs
    // (e.g. binary files).

    var buffer = blob.content();

    // If you know that the blob is UTF-8, however, 

    console.log(blob.toString());
  });

  // ### Revwalking

  // The libgit2 [revision walking api][rw] provides methods to traverse the
  // directed graph created by the parent pointers of the commit objects.
  // Since all commits point back to the commit that came directly before
  // them, you can walk this parentage as a graph and find all the commits
  // that were ancestors of (reachable from) a given starting point.  This
  // can allow you to create `git log` type functionality.

  var oid = git.Oid.fromString("f0877d0b841d75172ec404fc9370173dfffc20d1");

  // To use the revwalker, create a new walker, tell it how you want to sort
  // the output and then push one or more starting points onto the walker.
  // If you want to emulate the output of `git log` you would push the SHA
  // of the commit that HEAD points to into the walker and then start
  // traversing them.  You can also 'hide' commits that you want to stop at
  // or not see any of their ancestors.  So if you want to emulate `git log
  // branch1..branch2`, you would push the oid of `branch2` and hide the oid
  // of `branch1`.
  var revWalk = repo.createRevWalk();
  revWalk.sorting(git.RevWalk.Topological | git.RevWalkReverse);
  revWalk.push(oid);

  // Now that we have the starting point pushed onto the walker, we start
  // asking for ancestors. It will return them in the sorting order we asked
  // for as commit oids.  We can then lookup and parse the commited pointed
  // at by the returned OID; note that this operation is specially fast
  // since the raw contents of the commit object will be cached in memory

  function walk() {
    revWalk.next(function(error, oid) {
      if (error) throw error;
      if (!oid) return;

      repo.getCommit(oid, function(error, commit) {
        if (error) throw error;

        console.log(commit.sha());
        walk();
      });
    });
  }
  walk();

  // ### Index File Manipulation

  // The [index file API][gi] allows you to read, traverse, update and write
  // the Git index file (sometimes thought of as the staging area).
  repo.getIndex(function(error, index) {
    if (error) throw error;

    // For each entry in the index, you can get a bunch of information
    // including the SHA (oid), path and mode which map to the tree objects
    // that are written out.  It also has filesystem properties to help
    // determine what to inspect for changes (ctime, mtime, dev, ino, uid,
    // gid, file_size and flags) All these properties are exported publicly in
    // the `git_index_entry` struct

    index.entries().forEach(function(entry) {
      console.log(entry.path(), entry.mtime(), entry.size());
    });
  });

  // ### References

  // The [reference API][ref] allows you to list, resolve, create and update
  // references such as branches, tags and remote references (everything in
  // the .git/refs directory).

  repo.getReferences(function(error, references) {
    if (error) throw error;

    references.forEach(function(reference) {
      if (reference.type() == git.Reference.Oid) {
        console.log(oid.sha());
      } else if (reference.type() == git.Reference.Symbolic) {
        console.log(reference.symbolicTarget());
      }
    });
  });
});



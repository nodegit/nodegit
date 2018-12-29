var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);

describe("Tag", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;
  var Tag = NodeGit.Tag;
  var Obj = NodeGit.Object;
  var Oid = NodeGit.Oid;
  var Reference = NodeGit.Reference;
  var Signature = NodeGit.Signature;

  var reposPath = local("../repos/workdir");
  var tagName = "annotated-tag";
  var tagPattern = "annotated*";
  var tagFullName = "refs/tags/" + tagName;
  var tagOid = "dc800017566123ff3c746b37284a24a66546667e";
  var commitPointedTo = "32789a79e71fbc9e04d3eff7425e1771eb595150";
  var commitPointedTo2 = "c82fb078a192ea221c9f1093c64321c60d64aa0d";
  var tagMessage = "This is an annotated tag\n";

  function testTag(tag, name) {
    assert.equal(tag.name(), name || tagName);
    assert.equal(tag.targetType(), Obj.TYPE.COMMIT);
    assert.equal(tag.message(), tagMessage);

    return tag.target()
      .then(function(target) {
        assert.ok(target.isCommit());
        assert.equal(target.id().toString(), commitPointedTo);
      });
  }

  beforeEach(function() {
    var test = this;

    return Repository.open(reposPath)
      .then(function(repo) {
        test.repository = repo;
      });
  });

  it("can get a tag from a repo via the tag name", function() {
    return this.repository.getTagByName(tagName)
      .then(function(tag) {
        return testTag(tag);
      });
  });

  it("can get a tag from a repo via the long tag name", function() {
    return this.repository.getTagByName(tagFullName)
      .then(function(tag) {
        return testTag(tag);
      });
  });

  it("can get a tag from a repo via the tag's OID as a string", function() {
    return this.repository.getTag(tagOid)
      .then(function(tag) {
        return testTag(tag);
      });
  });

  it("can get a tag from a repo via the tag's OID object", function() {
    var oid = Oid.fromString(tagOid);

    return this.repository.getTag(oid)
      .then(function(tag) {
        return testTag(tag);
      });
  });

  it("can list tags in a repo", function() {
    return Tag.list(this.repository)
      .then(function(tagNames) {
        tagNames = tagNames.filter(function(tagNameTest) {
          return tagNameTest == tagName;
        });

        assert.equal(tagNames.length, 1);
      });
  });

  it("can list tags of a pattern in a repo", function() {
    return Tag.listMatch(tagPattern, this.repository)
      .then(function(tagNames) {
        assert.equal(tagNames.length, 1);
      });
  });

  it("can create a new annotated tag in a repo and delete it", function() {
    var oid = Oid.fromString(commitPointedTo);
    var name = "created-annotated-tag";
    var repository = this.repository;

    return repository.createTag(oid, name, tagMessage)
      .then(function(tag) {
        return testTag(tag, name);
      })
      .then(function() {
        return repository.createTag(oid, name, tagMessage);
      })
      .then(function() {
        return Promise.reject(new Error("should not be able to create the '" +
          name + "' tag twice"));
      }, function() {
        return Promise.resolve();
      })
      .then(function() {
        return repository.deleteTagByName(name);
      })
      .then(function() {
        return Reference.lookup(repository, "refs/tags/" + name);
      })
      .then(function() {
        return Promise.reject(new Error("the tag '" + name +
          "' should not exist"));
      }, function() {
        return Promise.resolve();
      });
  });

  it("can create a new lightweight tag in a repo and delete it", function() {
    var oid = Oid.fromString(commitPointedTo);
    var name = "created-lightweight-tag";
    var repository = this.repository;

    return repository.createLightweightTag(oid, name)
      .then(function(reference) {
        return reference.target();
      })
      .then(function(refOid) {
        assert.equal(refOid.toString(), oid.toString());
      })
      .then(function() {
        return repository.createLightweightTag(oid, name);
      })
      .then(function() {
        return Promise.reject(new Error("should not be able to create the '" +
          name + "' tag twice"));
      }, function() {
        return Promise.resolve();
      })
      .then(function() {
        return repository.deleteTagByName(name);
      })
      .then(function() {
        return Reference.lookup(repository, "refs/tags/" + name);
      })
      .then(function() {
        return Promise.reject(new Error("the tag '" + name +
          "' should not exist"));
      }, function() {
        return Promise.resolve();
      });
  });

  it("can create a new signed tag with Tag.create and delete it", function() {
    var name = "created-signed-tag-create";
    var repository = this.repository;
    var signature = Signature.default(repository);
    var commit = null;
    var commit2 = null;

    return repository.getCommit(commitPointedTo)
      .then(function(theCommit) {
        commit = theCommit;
        return repository.getCommit(commitPointedTo2);
      })
      .then(function(theCommit2) {
        commit2 = theCommit2;
        return Tag.create(repository, name, commit, signature, tagMessage, 1);
      })
      .then(function(oid) {
        return repository.getTag(oid);
      })
      .then(function(tag) {
        assert(tag.tagger(), signature);
        return testTag(tag, name);
      })
      .then(function() {
        // overwriting is okay
        return Tag.create(repository, name, commit2, signature, tagMessage, 1);
      })
      .then(function() {
        // overwriting is not okay
        return Tag.create(repository, name, commit, signature, tagMessage, 0);
      })
      .then(function() {
        return Promise.reject(new Error("should not be able to create the '" +
          name + "' tag twice"));
      }, function() {
        return Promise.resolve()
          .then(function() {
            return repository.deleteTagByName(name);
          })
          .then(function() {
            return Reference.lookup(repository, "refs/tags/" + name);
          })
          .then(function() {
            return Promise.reject(new Error("the tag '" + name +
              "' should not exist"));
          }, function() {
            return Promise.resolve();
          });
      });
  });

  it("can create a new signed tag with Tag.annotationCreate", function() {
    var oid = Oid.fromString(commitPointedTo);
    var name = "created-signed-tag-annotationCreate";
    var repository = this.repository;
    var signature = Signature.default(repository);
    var odb = null;

    return repository.odb()
      .then(function(theOdb) {
        odb = theOdb;
      })
      .then(function() {
        return Tag.annotationCreate(
          repository, name, oid, signature, tagMessage);
      })
      .then(function(oid) {
        return odb.read(oid);
      })
      .then(function(object) {
        assert(object.type(), Obj.TYPE.TAG);
      });
  });

  it("can peel a tag", function() {
    return this.repository.getTagByName(tagName)
      .then(function(tag) {
        return tag.peel();
      })
      .then(function(object) {
        assert.equal(object.isCommit(), true);
      });
  });

  it("can get tag's target id", function() {
    return this.repository.getTagByName(tagName)
      .then(function(tag) {
        assert.equal(commitPointedTo, tag.targetId());
      });
  });

  it("can get tag's owner", function() {
    var repository = this.repository;
    return this.repository.getTagByName(tagName)
      .then(function(tag) {
        var owner = tag.owner();
        assert.ok(owner instanceof Repository);
        assert.equal(repository.path(), owner.path());
      });
  });
});

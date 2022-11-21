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
    var signature = null;
    var commit = null;
    var commit2 = null;

    return Signature.default(repository)
      .then(function(signatureResult) {
        signature = signatureResult;
        return repository.getCommit(commitPointedTo);
      })
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

  it("can create a Tag buffer", function() {
    const targetOid = Oid.fromString(commitPointedTo);
    const name = "created-signed-tag-annotationCreate";
    const repository = this.repository;
    const signature = Signature.create(
      "Shaggy Rogers",
      "shaggy@mystery.com",
      987654321,
      90
    );
    const message = "I'm a teapot";

    return Tag.createBuffer(repository, name, targetOid, signature, message)
      .then((tagBuffer) => {
        const lines = tagBuffer.split("\n");
        assert.equal(7, lines.length);
        assert.equal(lines[0], `object ${commitPointedTo}`);
        assert.equal(lines[1], "type commit");
        assert.equal(lines[2], `tag ${name}`);
        assert.equal(
          lines[3],
          "tagger Shaggy Rogers <shaggy@mystery.com> 987654321 +0130"
        );
        assert.equal(lines[4], "");
        assert.equal(lines[5], message);
        assert.equal(lines[6], "");
      });
  });

  it("can create a Tag from a Tag buffer", function() {
    const targetOid = Oid.fromString(commitPointedTo);
    const otherTargetOid = Oid.fromString(commitPointedTo2);
    const name = "created-signed-tag-annotationCreate";
    const repository = this.repository;
    const signature = Signature.create(
      "Shaggy Rogers",
      "shaggy@mystery.com",
      987654321,
      90
    );
    const message = "I'm a teapot";

    let odb;
    let buffer;
    let otherBuffer;

    return repository.odb()
      .then((odbResult) => {
        odb = odbResult;
        return Tag.createBuffer(
          repository,
          name,
          targetOid,
          signature,
          message
        );
      })
      .then((bufferResult) => {
        buffer = bufferResult;
        return Tag.createBuffer(
          repository,
          name,
          otherTargetOid,
          signature,
          message
        );
      })
      .then((bufferResult) => {
        otherBuffer = bufferResult;
        return Tag.createFromBuffer(repository, buffer, 1);
      })
      .then((oid) => {
        return odb.read(oid);
      })
      .then((object) => {
        const lines = object.toString().split("\n");
        assert(object.type(), Obj.TYPE.TAG);
        assert.equal(7, lines.length);
        assert.equal(lines[0], `object ${commitPointedTo}`);
        assert.equal(lines[1], "type commit");
        assert.equal(lines[2], `tag ${name}`);
        assert.equal(
          lines[3],
          "tagger Shaggy Rogers <shaggy@mystery.com> 987654321 +0130"
        );
        assert.equal(lines[4], "");
        assert.equal(lines[5], message);
        assert.equal(lines[6], "");
      })
      .then(() => {
        // overwriting is okay
        return Tag.createFromBuffer(repository, otherBuffer, 1);
      })
      .then(() => {
        // overwriting is not okay
        return Tag.createFromBuffer(repository, buffer, 0);
      })
      .then(() => {
        return Promise.reject(
          new Error("should not be able to create the '" + name + "' tag twice")
        );
      },
      () => {
        return Promise.resolve();
      });
  });

  describe("createWithSignature and extractSignature", function() {
    it(
      "can create a tag with a signature and extract the signature",
      function() {
        var targetCommit;
        var otherTargetCommit;
        const name = "created-signed-tag-annotationCreate";
        const repository = this.repository;
        const signature = Signature.create(
          "Shaggy Rogers",
          "shaggy@mystery.com",
          987654321,
          90
        );
        const signatureLines = [
          "-----BEGIN PGP SIGNATURE-----",
          "iQIzBAABCAAdFiEEKdxGpJ93wnkLaBKfURjJKedOfEMFAlxR4JUACgkQURjJKedO",
          "fEN+8A//cXmkRmhzQMdTEdrxty7tVKQ7lVhL7r7e+cB84hO7WrDn8549c7/Puflu",
          "idanWfyoAEMSNWDgY84lx/t3I3YYKXsLDPT93HiMhCXmPVZcfLxlARRL1rrNZV4q",
          "L9hhqb9bFrRNBn6YebhygeLXLHlDKEZzx8W9jnDLU8Px8UTkwdQIDnPDfT7UOPPU",
          "MYDgP3OwWwoG8dUlZXaHjtFz29wPlJo177MwdLYwn4zpEIysoY1ev5IKWD+LPW4g",
          "vdQnaK1x3dozmG8YLUZw5iW7ap9DpahbAGQgdy1z1ypiNUjNuhaP8zkG1ci6X88N",
          "6MIoQ+YqfowRJJTIr1lzssxsRI1syjfS6smnI4ZNE6S+6mIKN96ES2OZF+rn4xnD",
          "PofR9Qh2gPq++ULriPE/cX7ZkZ0/ZDZGDfIGvricB8JEJhISZn/VMX/KScJs+rFq",
          "KWN5Au6Uc2pEqeq5OP4y2k0QUmKQT9sh9OepnPmfqF8hG6wI8nM67jT/FEOcpr0v",
          "qoN2NRXrcq3iZAp07AGq9IdpYhBcEW7MFmOcNt+Zb8SbTMp6DawnREg9xzz1SIkZ",
          "Cdp1XoJ6mkVvzBB4T/Esp7j1VztinTX2PpX7C1CE5LC76UfCiEjEWOmWrVuPuA5a",
          "oRrJvgPJg8gpVj04r2m8nvUK1gwhxg9ZB+SK+nd3OAd0dnbJwTE=",
          "=dW3g",
          "-----END PGP SIGNATURE-----"
        ];
        const message = "I'm a teapot";
        const signingCallback = (message) => ({
          code: NodeGit.Error.CODE.OK,
          signedData: signatureLines.join("\n")
        });

        let odb;
        let oid;
        let object;

        return repository.getCommit(commitPointedTo).then((commit) => {
          targetCommit = commit;
          return repository.getCommit(commitPointedTo2);
        }).then((commit) => {
          otherTargetCommit = commit;
          return repository.odb();
        }).then((odbResult) => {
          odb = odbResult;

          return Tag.createWithSignature(
            repository,
            name,
            targetCommit,
            signature,
            message,
            1,
            signingCallback
          );
        })
        .then((oidResult) => {
          oid = oidResult;
          return odb.read(oid);
        })
        .then((objectResult) => {
          object = objectResult;
          const lines = object.toString().split("\n");
          assert(object.type(), Obj.TYPE.TAG);
          assert.equal(signatureLines.length + 7, lines.length);
          assert.equal(lines[0], `object ${commitPointedTo}`);
          assert.equal(lines[1], "type commit");
          assert.equal(lines[2], `tag ${name}`);
          assert.equal(
            lines[3],
            "tagger Shaggy Rogers <shaggy@mystery.com> 987654321 +0130"
          );
          assert.equal(lines[4], "");
          assert.equal(lines[5], message);
          for (let i = 6; i < 6 + signatureLines.length; i++) {
            assert.equal(lines[i], signatureLines[i - 6]);
          }
          assert.equal(lines[6 + signatureLines.length], "");

          return Tag.lookup(repository, oid);
        })
        .then((tag) => {
          return tag.extractSignature();
        })
        .then((tagSignature) => {
          assert.equal(tagSignature, signatureLines.join("\n"));
        })
        .then(() => {
          // overwriting is okay
          return Tag.createWithSignature(
            repository,
            name,
            targetCommit,
            signature,
            message,
            1,
            signingCallback
          );
        })
        .then(() => {
          // overwriting is not okay
          return Tag.createWithSignature(
            repository,
            name,
            otherTargetCommit,
            signature,
            message,
            0,
            signingCallback
          );
        })
        .then(() => {
          return Promise.reject(
            new Error(
              "should not be able to create the '" + name + "' tag twice"
            )
          );
        },
        () => {
          return Promise.resolve();
        });
      }
    );

    it("can optionally skip the signing process", function() {
      var targetCommit;
      var otherTargetCommit;
      const name = "created-signed-tag-annotationCreate";
      const repository = this.repository;
      const signature = Signature.create(
        "Shaggy Rogers",
        "shaggy@mystery.com",
        987654321,
        90
      );
      const message = "I'm a teapot";
      const signingCallback = () => ({
        code: NodeGit.Error.CODE.PASSTHROUGH
      });

      let odb;
      let oid;
      let object;

      return repository.getCommit(commitPointedTo).then((commit) => {
        targetCommit = commit;
        return repository.getCommit(commitPointedTo2);
      }).then((commit) => {
        otherTargetCommit = commit;
        return repository.odb();
      }).then((odbResult) => {
        odb = odbResult;

        return Tag.createWithSignature(
          repository,
          name,
          targetCommit,
          signature,
          message,
          1,
          signingCallback
        );
      })
      .then((oidResult) => {
        oid = oidResult;
        return odb.read(oid);
      })
      .then((objectResult) => {
        object = objectResult;
        const lines = object.toString().split("\n");
        assert(object.type(), Obj.TYPE.TAG);
        assert.equal(7, lines.length);
        assert.equal(lines[0], `object ${commitPointedTo}`);
        assert.equal(lines[1], "type commit");
        assert.equal(lines[2], `tag ${name}`);
        assert.equal(
          lines[3],
          "tagger Shaggy Rogers <shaggy@mystery.com> 987654321 +0130"
        );
        assert.equal(lines[4], "");
        assert.equal(lines[5], message);
        assert.equal(lines[6], "");

        return Tag.lookup(repository, oid);
      })
      .then((tag) => {
        return tag.extractSignature();
      })
      .then(function() {
        assert.fail("Tag should not have been signed.");
      }, function(error) {
        if (error && error.message === "this tag is not signed") {
          return;
        }

        throw error;
      })
      .then(() => {
        // overwriting is okay
        return Tag.createWithSignature(
          repository,
          name,
          targetCommit,
          signature,
          message,
          1,
          signingCallback
        );
      })
      .then(() => {
        // overwriting is not okay
        return Tag.createWithSignature(
          repository,
          name,
          otherTargetCommit,
          signature,
          message,
          0,
          signingCallback
        );
      })
      .then(() => {
        return Promise.reject(
          new Error("should not be able to create the '" + name + "' tag twice")
        );
      },
      () => {
        return Promise.resolve();
      });
    });

    it("will throw if signing callback returns an error code", function() {
      var targetCommit;
      const name = "created-signed-tag-annotationCreate";
      const repository = this.repository;
      const signature = Signature.create(
        "Shaggy Rogers",
        "shaggy@mystery.com",
        987654321,
        90
      );
      const message = "I'm a teapot";
      const signingCallback = () => ({
        code: NodeGit.Error.CODE.ERROR
      });


      return repository.getCommit(commitPointedTo).then((commit) => {
        targetCommit = commit;
        return Tag.createWithSignature(
          repository,
          name,
          targetCommit,
          signature,
          message,
          1,
          signingCallback
        );
      }).then(function() {
          assert.fail("Should not have been able to create tag");
        }, function(error) {
          if (error && error.errno === NodeGit.Error.CODE.ERROR) {
            return;
          }
          throw error;
        });
    });
  });

  it("will show a deprecation warning if createWithSignature use oid instead object", function() {
    var targetCommit;
    const name = "created-signed-tag-annotationCreate";
    const repository = this.repository;
    const signature = Signature.create(
      "Shaggy Rogers",
      "shaggy@mystery.com",
      987654321,
      90
    );
    const message = "I'm a teapot";
    const signingCallback = () => ({
      code: NodeGit.Error.CODE.ERROR
    });


    return repository.getCommit(commitPointedTo).then((commit) => {
      targetCommit = commit;
      return Tag.createWithSignature(
        repository,
        name,
        targetCommit.id(),
        signature,
        message,
        1,
        signingCallback
      );
    }).then(function() {
        assert.fail("Should not have been able to create tag");
      }, function(error) {
        if (error && error.errno === NodeGit.Error.CODE.ERROR) {
          return;
        }
        throw error;
      });
  });

  it("can create a new signed tag with Tag.annotationCreate", function() {
    var targetCommit;
    var name = "created-signed-tag-annotationCreate";
    var repository = this.repository;
    var signature = null;
    var odb = null;

    return repository.getCommit(commitPointedTo).then((commit) => {
      targetCommit = commit;
      return Signature.default(repository);
    }).then(function(signatureResult) {
        signature = signatureResult;
        return repository.odb();
      })
      .then(function(theOdb) {
        odb = theOdb;
      })
      .then(function() {
        return Tag.annotationCreate(
          repository, name, targetCommit, signature, tagMessage);
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

var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);

describe("Tag", function() {
  var Repository = require(local("../../lib/repository"));
  var Tag = require(local("../../lib/tag"));
  var Obj = require(local("../../lib/object"));
  var Oid = require(local("../../lib/oid"));

  var reposPath = local("../repos/workdir/.git");
  var tagName = "annotated-tag";
  var tagFullName = "refs/tags/" + tagName;
  var tagOid = "dc800017566123ff3c746b37284a24a66546667e";
  var commitPointedTo = "32789a79e71fbc9e04d3eff7425e1771eb595150";
  var tagMessage = "This is an annotated tag\n";

  function testTag(tag, name) {
    assert.equal(tag.name(), name || tagName);
    assert.equal(tag.targetType(), Obj.TYPE.COMMIT);
    assert.equal(tag.message(), tagMessage);

    var target = tag.target();

    assert.ok(target.isCommit());
    assert.equal(target.id().toString(), commitPointedTo);
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
        testTag(tag);
      });
  });

  it("can get a tag from a repo via the long tag name", function() {
    return this.repository.getTagByName(tagFullName)
      .then(function(tag) {
        testTag(tag);
      });
  });

  it("can get a tag from a repo via the tag's OID as a string", function() {
    return this.repository.getTag(tagOid)
      .then(function(tag) {
        testTag(tag);
      });
  });

  it("can get a tag from a repo via the tag's OID object", function() {
    var oid = Oid.fromString(tagOid);

    return this.repository.getTag(oid)
      .then(function(tag) {
        testTag(tag);
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
});

var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);

describe("Note", function() {
  var NodeGit = require("../../");
  var Note = NodeGit.Note;
  var Signature = NodeGit.Signature;
  var reposPath = local("../../");

  beforeEach(function() {
    var test = this;

    return NodeGit.Repository.open(reposPath).then(function(repository) {
      test.repository = repository;

      return repository.getMasterCommit().then(function(commit) {
        test.commit = commit; 
      });
    });
  });

  it("can be created", function() {
    var sha = this.commit.id();
    var sig = Signature.create("John", "john@doe.com", Date.now(), 0);
    var noteRef = "refs/notes/commits";

    return Note.create(this.repository, noteRef, sig, sig, sha, "Testing!", 1);
  });

  it("can be read", function() {
    var sha = this.commit.id();
    var noteRef = "refs/notes/commits";

    return Note.read(this.repository, noteRef, sha).then(function(note) {
      assert.equal(note.message(), "Testing!");
    });
  });

  it("can iterate all notes", function() {
    var test = this;
    var noteRef = "refs/notes/commits";
    var ref = null;

    return Note.foreach(this.repository, noteRef, function(blobId, objectId) {
      ref = objectId;
    }).then(function() {
      return NodeGit.Note.read(test.repository, noteRef, ref)
        .then(function(note) {
          assert.equal(note.message(), "Testing!");
        });
    });
  });

  it("can be removed", function(done) {
    var test = this;
    var sha = this.commit.id();
    var noteRef = "refs/notes/commits";
    var sig = Signature.create("John", "john@doe.com", Date.now(), 0);

    return Note.remove(this.repository, noteRef, sig, sig, sha)
      .then(function() {
        return Note.read(test.repository, noteRef, sha).catch(function(ex) {
          assert.equal(ex.message, "Note could not be found");
          done();
        });
      });
  });
});

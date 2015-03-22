var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);

describe("Note", function() {
  var NodeGit = require("../../");
  var Note = NodeGit.Note;
  var Signature = NodeGit.Signature;
  var reposPath = local("../repos/merge");

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

  it("can be removed", function(done) {
    var sha = this.commit.id();
    var noteRef = "refs/notes/commits";
    var sig = Signature.create("John", "john@doe.com", Date.now(), 0);

    Note.remove(this.repository, noteRef, sig, sig, sha);

    return Note.read(this.repository, noteRef, sha).catch(function(ex) {
      assert.equal(ex.message, "Note could not be found");
      done();
    });
  });
});

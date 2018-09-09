var assert = require("assert");
var repoSetup = require("../utils/repository_setup");
var fse = require("fs-extra");
var path = require("path");
var local = path.join.bind(path, __dirname);

describe("ConvenientLine", function() {
  var repoPath = local("../repos/convenientLineTest");
  var unicodeLine = "Ťḥ𝖎ṧ ℓỈ𝓃ệ çǒ𝚗ẗảḭṋṦ Û𝐧ǐ𝗰ṓḍ𝔢\n";
  var asciiLine = "but this line doesn't\n";

  beforeEach(function() {
    var test = this;

    return repoSetup.createRepository(repoPath)
      .then(function(repo) {
        return repoSetup.commitFileToRepo(
          repo,
          "fileWithUnicodeChars",
          unicodeLine + asciiLine
        );
      })
      .then(function(commit) {
        return commit.getDiff();
      })
      .then(function(diff) {
        return diff[0].patches();
      })
      .then(function(patches) {
        return patches[0].hunks();
      })
      .then(function(hunks) {
        return hunks[0].lines();
      })
      .then(function(lines) {
        test.unicodeLine = lines[0];
        test.asciiLine = lines[1];
      });
  });

  after(function() {
    return fse.remove(repoPath);
  });

  it("can parse the byte length of a unicode string", function() {
    var line = this.unicodeLine;

    assert.equal(line.contentLen(), Buffer.byteLength(unicodeLine, "utf8"));
  });

  it("can get a line that contains unicode", function() {
    var line = this.unicodeLine;

    assert.equal(line.content(), unicodeLine);
  });

  it("can parse the byte length of a ascii string", function() {
    var line = this.asciiLine;

    assert.equal(line.contentLen(), Buffer.byteLength(asciiLine, "utf8"));
  });

  it("can get a line that contains ascii", function() {
    var line = this.asciiLine;

    assert.equal(line.content(), asciiLine);
  });
});

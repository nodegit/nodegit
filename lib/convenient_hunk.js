function ConvenientHunk(raw, i) {
  this.raw = raw;
  this.i = i;
}

ConvenientHunk.prototype.header = function() {
  return this.raw.hunk(this.i).header;
};

ConvenientHunk.prototype.size = function() {
  return this.raw.hunk(this.i).lines;
};

ConvenientHunk.prototype.lines = function() {
  var result = [];
  for (var i = 0; i < this.size(); i++)
    result.push(this.raw.line(this.i, i));
  return result;
};

exports.ConvenientHunk = ConvenientHunk;

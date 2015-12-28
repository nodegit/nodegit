var NodeGit = require("../");

function ConvenientLine(raw, i) {
  this.raw = raw;
  this.i = i;
  this._cache = {};
}

/**
* The type of the line (context, addition, deletion, etc...)
* @return {Diff.LINE}
*/
ConvenientLine.prototype.origin = function() {
 return this.raw.origin();
};

/**
* Line number in old file or -1 for added line
* @return {Number}
*/
ConvenientLine.prototype.oldLineno = function() {
 return this.raw.oldLineno();
};

/**
* Line number in new file or -1 for deleted line
* @return {Number}
*/
ConvenientLine.prototype.newLineno = function() {
 return this.raw.newLineno();
};

/**
* Number of newline characters in content
* @return {Number}
*/
ConvenientLine.prototype.numLines = function() {
 return this.raw.numLines();
};

/**
* Number of bytes in the string
* @return {Number}
*/
ConvenientLine.prototype.contentLen = function() {
  return this.raw.contentLen();
};

/**
* Offset in the original file to the content
* @return {Number}
*/
ConvenientLine.prototype.contentOffset = function() {
 return this.raw.contentOffset();
};

/**
* Pointer to diff text, not NUL-byte terminated
* @return {String}
*/
ConvenientLine.prototype.rawContent = function() {
  return this.raw.content();
};

/**
* The relevant line
* @return {String}
*/
ConvenientLine.prototype.content = function() {
  if (!this._cache.content) {
    this._cache.content = new Buffer(this.raw.content())
      .slice(0, this.raw.contentLen())
      .toString("utf8");
  }

  return this._cache.content;
};

NodeGit.ConvenientLine = ConvenientLine;

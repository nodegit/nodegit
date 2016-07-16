var NodeGit = require("../");
var DiffLine = NodeGit.DiffLine;

/**
* The relevant line
* @return {String}
*/
DiffLine.prototype.content = function() {
  if (!this._cache) {
    this._cache = {};
  }

  if (!this._cache.content) {
    this._cache.content = new Buffer(this.rawContent())
      .slice(0, this.contentLen())
      .toString("utf8");
  }

  return this._cache.content;
};

/**
* The non utf8 translated text
* @return {String}
*/
var rawContent = DiffLine.prototype.content;
DiffLine.prototype.rawContent = function() {
  return rawContent.call(this);
};

NodeGit.DiffLine = DiffLine;

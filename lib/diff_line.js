var NodeGit = require("../");
var DiffLine = NodeGit.DiffLine;

/**
* The non utf8 translated text
* @return {String}
*/
var rawContent = DiffLine.prototype.content;
DiffLine.prototype.rawContent = function() {
  return rawContent.call(this);
};

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

NodeGit.DiffLine = DiffLine;

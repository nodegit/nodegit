var NodeGit = require("../");

function ConvenientLine(raw, i) {
  this.raw = raw;
  this.i = i;
}

/**
* The content of the line
* @return {String}
*/
ConvenientLine.prototype.origin = function() {
 return this.raw.origin();
};

/**
* The content of the line
* @return {String}
*/
ConvenientLine.prototype.oldLineno = function() {
 return this.raw.oldLineno();
};

/**
* The content of the line
* @return {String}
*/
ConvenientLine.prototype.newLineno = function() {
 return this.raw.newLineno();
};

/**
* The content of the line
* @return {String}
*/
ConvenientLine.prototype.numLines = function() {
 return this.raw.numLines();
};

/**
* The content of the line
* @return {String}
*/
ConvenientLine.prototype.contentLen = function() {
 return this.raw.contentLen();
};


/**
* The content of the line
* @return {String}
*/
ConvenientLine.prototype.contentOffset = function() {
 return this.raw.contentOffset();
};

/**
* The content of the line
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
  return this.raw.content()
    .substring(0, this.raw.contentLen()).replace("\n", "");
};

NodeGit.ConvenientLine = ConvenientLine;

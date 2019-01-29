const { Buf } = require("../");

/**
 * Sets the content of a GitBuf to a string.
 * @param {string} The utf8 value to set in the buffer.
 *                 The string will be null terminated.
 */
Buf.prototype.setString = function(content) {
  const buf = Buffer.from(content + "\0", "utf8");
  this.set(buf, buf.length);
};

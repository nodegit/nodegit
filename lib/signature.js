var NodeGit = require("../");
var Signature = NodeGit.Signature;

const toPaddedDoubleDigitString = (number) => {
  if (number < 10) {
    return `0${number}`;
  }

  return `${number}`;
};

/**
 * Standard string representation of an author.
 * @param {Boolean} withTime Whether or not to include timestamp
 * @return {String} Representation of the author.
 */
Signature.prototype.toString = function(withTime) {
  const name = this.name().toString();
  const email = this.email().toString();

  let stringifiedSignature = `${name} <${email}>`;

  if (!withTime) {
    return stringifiedSignature;
  }

  const when = this.when();
  const offset = when.offset();
  const offsetMagnitude = Math.abs(offset);
  const time = when.time();

  const sign = (offset < 0 || when.sign() === "-") ? "-" : "+";
  const hours = toPaddedDoubleDigitString(Math.floor(offsetMagnitude / 60));
  const minutes = toPaddedDoubleDigitString(offsetMagnitude % 60);

  stringifiedSignature += ` ${time} ${sign}${hours}${minutes}`;
  return stringifiedSignature;
};

module.exports = function(args, fieldToRetrieve) {
  const thisArg = args.find(arg => arg.isSelf);
  if (thisArg) {
    return thisArg[fieldToRetrieve];
  }

  return;
};

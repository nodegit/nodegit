var git = require('../');

/**
 * Apply given details to the context.
 *
 * @param  {Object} details
 * @param  {Object} context
 * @return {Object} The modified context.
 */
exports.applyProperties = function(details, context) {
  if (details) {
    for (var detailKey in details) {
      if (detailKey === 'id' && details[detailKey] instanceof git.raw.Oid) {
        context[detailKey] = git.oid(details[detailKey]);
        continue;
      }
      if (detailKey === 'object' && details[detailKey] instanceof git.raw.Object) {
        context[detailKey] = git.object(details[detailKey]);
        continue;
      }
      context[detailKey] = details[detailKey];
    }
  }
  return context;
};

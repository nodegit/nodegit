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
      if (detailKey === 'id') {
        context[detailKey] = git.oid(details[detailKey]);
        continue;
      }
      context[detailKey] = details[detailKey];
    }
  }
  return context;
};

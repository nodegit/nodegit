var git = require('../'),
    path = require('path');

git.Repo.open(path.resolve(__dirname, '../.git'), function(error, repo) {
  if (error) throw error;

  var remote = repo.getRemote("origin");
  remote.connect(0, function(error) {
    if (error) throw error;

    remote.download(null, function(error) {
      if (error) throw error;

      console.log("It worked!");
    })
  });
});

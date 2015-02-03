var nodegit = require("../"),
    path = require("path");

// This code shows working directory changes similar to git status

nodegit.Repository.open(path.resolve(__dirname, "../.git"))
  .then(function(repo) {
    repo.getStatus().then(function(statuses) {
      function statusToText(status) {
        var words = [];
        if (status.isNew()) { words.push("NEW"); }
        if (status.isModified()) { words.push("MODIFIED"); }
        if (status.isTypechange()) { words.push("TYPECHANGE"); }
        if (status.isRenamed()) { words.push("RENAMED"); }
        if (status.isIgnored()) { words.push("IGNORED"); }

        return words.join(" ");
      }

      statuses.forEach(function(file) {
        console.log(file.path() + " " + statusToText(file));
      });
    });
});

var nodegit = require("../");
var path = require("path");
var fse = require("fs-extra");

nodegit.Repository.open(path.resolve(__dirname, "../.git"))
  .then(function(repo) {
    return repo.refreshIndex()
    .then(function(index) {
      var fileContent = {
        newFile1: "this has some content",
        newFile2: "and this will have more content"
      };
      var fileNames = Object.keys(fileContent);

      return Promise.all(fileNames.map(function(fileName) {
        fse.writeFile(
          path.join(repo.workdir(), fileName), fileContent[fileName]);
      }))



        // This will add all files to the index
      .then(function() {
        return index.addAll();
      })
      .then(function() {
        var newFiles = index.entries().filter(function(entry) {
          return ~fileNames.indexOf(entry.path);
        });

        console.log(
          "\n-------------------\n" +
          "Added files: " +
          "\n-------------------\n");
        newFiles.forEach(function(entry) {
          console.log(entry.path);
        });
      })
      .then(function() {
        // This will remove the files from the index
        return index.removeAll("newFile*");
      })
      .then(function() {
        var newFiles = index.entries().filter(function(entry) {
          return ~fileNames.indexOf(entry.path);
        });

        console.log("New files in index: " + newFiles.length);
      })



      // We can also provide a pattern to add files to the index
      .then(function() {
        return index.addAll("newFile*");
      })
      .then(function() {
        var newFiles = index.entries().filter(function(entry) {
          return ~fileNames.indexOf(entry.path);
        });

        console.log(
          "\n-------------------\n" +
          "Added files with pattern: " +
          "\n-------------------\n");
        newFiles.forEach(function(entry) {
          console.log(entry.path);
        });
      })
      .then(function() {
        // We're also using the pattern in the remove
        return index.removeAll("newFile*");
      })
      .then(function() {
        var newFiles = index.entries().filter(function(entry) {
          return ~fileNames.indexOf(entry.path);
        });

        console.log("New files in index: " + newFiles.length);
      })



      // Callbacks can be used for a finer degree of control over what
      // we add to the index
      .then(function() {
        return index.addAll(
          "newFile*",
          nodegit.Index.ADD_OPTION.ADD_CHECK_PATHSPEC,
          function(path, matchedPattern) {
            if (path == "newFile1") {
              return 0; // add the file
            }

            return 1; // skip the file
          });
      })
      .then(function() {
        var newFiles = index.entries().filter(function(entry) {
          return ~fileNames.indexOf(entry.path);
        });

        console.log(
          "\n-------------------\n" +
          "Added files with callback: " +
          "\n-------------------\n");
        newFiles.forEach(function(entry) {
          console.log(entry.path);
        });
      })
      .then(function() {
        // Lets use a callback in the remove as well
        return index.removeAll(null, function(path) {
          if (~path.indexOf("newFile")) {
            return 0; // remove the file
          }

          return 1; // don't remove the file
        });
      })
      .then(function() {
        var newFiles = index.entries().filter(function(entry) {
          return ~fileNames.indexOf(entry.path);
        });

        console.log("Total: " + index.entries().length);
        console.log("New files in index: " + newFiles.length);
      });
    });
  }).done(function() {
    console.log("All done!");
  });

var assert = require("assert"),
  promisify = require("promisify-node"),
  fse = promisify(require("fs-extra")),
  path = require("path"),
  local = path.join.bind(path, __dirname);

describe.only("Filter", function() {
  var NodeGit = require("../../");

  var emptyRepoPath = local("../repos/empty"),
    filterName = "psuedo_filter",
    // newRepoPath = local("../repos/newrepo"),
    Registry = NodeGit.FilterRegistry,
    Checkout = NodeGit.Checkout,
    Repository = NodeGit.Repository,
    Attr = NodeGit.Attr,
    Status = NodeGit.Status,
    reposPath = local("../repos/workdir");

  beforeEach(function() {
    var test = this;

    return Repository.open(reposPath)
      .then(function(repository) {
        test.repository = repository;
      })
      .then(function() {
        return Repository.open(emptyRepoPath);
      })
      .then(function(emptyRepo) {
        test.emptyRepo = emptyRepo;
      });
  });

  afterEach(function() {
    //Unregistering the filter to avoid GIT_EEXISTS
    Registry.unregister(filterName);
  });

  it("Registering filter for the first time", function() {
    // registering custom filter
    var result = Registry.register(filterName, {
      initialize: function() {
        console.log("inside INIT");
      },
      apply: function() {
        console.log("inside APPLY");
      },
      stream: function() {
        console.log("inside STREAM");
      },
      check: function(){
        console.log("inside CHECK");
      }
    }, 0);
    assert.strictEqual(result, 0);
  });

  it("Registering filter and re-registering same filter", function() {
    // registering custom filter
    var result = Registry.register(filterName, {
      initialize: function() {
        console.log("inside INIT");
      },
      apply: function() {
        console.log("inside APPLY");
      },
      stream: function() {
        console.log("inside STREAM");
      },
      check: function(){
        console.log("inside CHECK");
      }
    }, 0);
    assert.strictEqual(result, 0);

    result = Registry.register(filterName, {
      initialize: function() {
        console.log("inside INIT");
      },
      apply: function() {
        console.log("inside APPLY");
      },
      stream: function() {
        console.log("inside STREAM");
      },
      check: function(){
        console.log("inside CHECK");
      }
    }, 0);
    assert.strictEqual(result, -4);
  });

  it("Testing Initialize callback", function() {
    var test = this,
      testFilePath = path.join(reposPath, "package.json"),
      flags = Status.SHOW.INDEX_AND_WORKDIR;

    // registering custom filter
    var result = Registry.register(filterName, {
      initialize: function() {
        console.log("inside INIT");
      },
      apply: function() {
        console.log("inside APPLY");
      },
      stream: function() {
        console.log("inside STREAM");
      },
      check: function(){
        console.log("inside CHECK");
      }
    }, 0);
    
    assert.strictEqual(result, 0);

    // creating .gitattributes
    var gitattributeFilePromise = fse.writeFile(
      path.join(reposPath, ".gitattributes"), 
      "* filter="+ filterName +" diff=lfs merge=lfs -text", {
      encoding: "utf-8",
    });
    //creating test file that will be used to trigger custom filter
    var testFilePromise = fse.writeFile(
      testFilePath, 
      "initial text", {
      encoding: "utf-8",
    });

    Attr.cacheFlush(this.repository);

    // setup complete, testing initialize of custom filter
    return Promise.all([gitattributeFilePromise, testFilePromise])
      // create necessary files
      .then(function() {
        return Attr.get(
          test.repository, 
          flags, 
          path.join(reposPath, ".gitattributes"), 
          "filter");
      })
      // check attribute values
      .then(function(data) {
        console.log("data: ", data);
        assert.strictEqual(data, filterName);
      })
      // modify file
      .then(function() {
        return fse.writeFile(testFilePath, 
        "Modified Content", 
        {
          encoding: "utf-8"
        });
      })
      // perform checkout, which should trigger filter
      .then(function() {
        var opts = {
          checkoutStrategy: Checkout.STRATEGY.FORCE,
          paths: "package.json"
        };
        return Checkout.head(test.repository, opts);
      })
      .then(function() {
        console.log("Post checkout");
      });
  });

});

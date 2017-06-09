var assert = require("assert"),
  promisify = require("promisify-node"),
  fse = promisify(require("fs-extra")),
  path = require("path"),
  local = path.join.bind(path, __dirname);

describe("Filter", function() {
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

  describe.only("Register and unregister", function(){
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

  });

  describe("Initialize callback", function(){

    it.only("Testing Initialize callback", function() {
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
        check: function(){
          console.log("inside CHECK");
          return 0;
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

  describe("Shutdown callback", function(){

    it("Testing shutdown callback", function(){
      var result = Registry.register(filterName, {
        initialize: function() {
          console.log("inside INIT");
          return 0;
        },
        apply: function() {
          console.log("inside APPLY");
        },
        stream: function() {
          console.log("inside STREAM");
        },
        check: function(){
          console.log("inside CHECK");
          return 0;
        },
        shutdown: function(){
          console.log("inside SHUTDOWN");
          return 0;
        }
      }, 0);
      assert.strictEqual(result, 0);

      result = Registry.unregister(filterName);
      assert.strictEqual(result, 0);
    });

    // TODO: shutdown is supposed to work even if
    // intialize call back is not provided/call not made
    it("Testing shutdown callback with initialize callback", function(){
      var result = Registry.register(filterName, {
        apply: function() {
          console.log("inside APPLY");
        },
        stream: function() {
          console.log("inside STREAM");
        },
        check: function(){
          console.log("inside CHECK");
          return 0;
        },
        shutdown: function(){
          console.log("inside SHUTDOWN");
          return 0;
        }
      }, 0);
      assert.strictEqual(result, 0);

      result = Registry.unregister(filterName);
      // somewhere here test the result of the shutdown callback
      assert.strictEqual(result, 0);
    });
    
  });

  describe("Check Callback", function(){

    it("Normal check usage", function(){
      var result = Registry.register(filterName, {
        initialize: function() {
          console.log("inside INIT");
          return 0;
        },
        apply: function() {
          console.log("inside APPLY");
        },
        stream: function() {
          console.log("inside STREAM");
        },
        check: function(){
          console.log("inside CHECK");
          return 0;
        },
        shutdown: function(){
          console.log("inside SHUTDOWN");
          return 0;
        }
      }, 0);
      assert.strictEqual(result, 0);
      // do some action like change package.json
      // checkout -> clean -> to odb
      assert.strictEqual(result, 0);
    });

    it(
      "GIT_PASSTHROUGH should be returned" +
      " if filter is not to be applied", function(){
      var result = Registry.register(filterName, {
        initialize: function() {
          console.log("inside INIT");
          return 0;
        },
        apply: function() {
          console.log("inside APPLY");
        },
        stream: function() {
          console.log("inside STREAM");
        },
        check: function(){
          console.log("inside CHECK");
          return 0;
        },
        shutdown: function(){
          console.log("inside SHUTDOWN");
          return 0;
        }
      }, 0);
      assert.strictEqual(result, 0);
      /* 
      do some action like change package.json
      the filter should return GIT_PASSTHROUGH
      verify -> apply callback should get -30 or anything except 0
      check if contents of file modified are not changed and staged?
      */
    });
  });

  describe("Apply Callback", function(){
    it("Verify apply callback", function(){
      /*
      register filter
      make changes -> checkout
      verify return value from apply
      verify changes
      */
    });
    it("Verify apply callback did not work", function(){
      /*
      register filter
      make changes -> checkout
      verify return value from apply as GIT_PASSTHROUGH
      verify changes
      */
    });
  });

});

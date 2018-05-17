var nodegit = require("../");
var path = "tmp/nodegit-clone-demo";

var config = {
	fetchOpts: {
		callbacks: {
			certificateCheck: function() {
				// github will fail cert check on some OSX machines
				// this overrides that check
				return 1;
			}
		}
	}
};

nodegit.Clone("https://github.com/nodegit/nodegit.git", path, config)
	.then(function(repo){
		console.log(1);
		var p = Promise.resolve();
		nodegit.Submodule.foreach(repo, function(submodule){
			submodule.update(1, config);
			p = p.then(function(){
				return new Promise(function(resolve, reject){
					submodule.update(1, config).then(resolve, reject);
				});
			});
		}).then(function(){
			p.then(function(){
				console.log("Cloned!");
			},function(error){
				console.error(error);
			});
		});
	})
	.catch(console.log);
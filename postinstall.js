#!/usr/bin/env node

var fs = require("fs");
var child_process = require("child_process");

if (process.platform !== "linux") {
	return;
}

child_process.exec("node lib/nodegit.js", function(error, stdout, stderr) {
	if (stderr && ~stderr.indexOf("libstdc++")) {
		console.log("[ERROR] Seems like the latest libstdc++ is missing on your system!");
		console.log("");
		console.log("On Ubuntu you can install it using:");
		console.log("");
		console.log("$ sudo add-apt-repository ppa:ubuntu-toolchain-r/test");
		console.log("$ sudo apt-get update");
		console.log("$ sudo apt-get install libstdc++-4.9-dev");
	}
});

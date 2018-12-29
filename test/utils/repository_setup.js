var assert = require("assert");
var NodeGit = require("../../");
var path = require("path");
var fse = require("fs-extra");

var RepositorySetup = {
	addFileToIndex:
	function addFileToIndex(repository, fileName) {
		return repository.refreshIndex()
			.then(function(index) {
				return index.addByPath(fileName)
					.then(function() {
						return index.write();
					})
					.then(function() {
						return index.writeTree();
					});
			});
	},

	commitFileToRepo:
	function commitFileToRepo(repository, fileName, fileContent, parentCommit) {
		var repoWorkDir = repository.workdir();
		var signature = NodeGit.Signature.create("Foo bar",
	    "foo@bar.com", 123456789, 60);

		var filePath = path.join(repoWorkDir, fileName);
		var parents = [];
		if (parentCommit) {
			parents.push(parentCommit);
		}

		// fse.ensure allows us to write files inside new folders
		return fse.ensureFile(filePath)
			.then(function() {
				return fse.writeFile(filePath, fileContent);
			})
			.then(function() {
				return RepositorySetup.addFileToIndex(repository, fileName);
			})
			.then(function(oid) {
				return repository.createCommit("HEAD", signature, signature,
					"initial commit", oid, parents);
			})
			.then(function(commitOid) {
				return repository.getCommit(commitOid);
			});
	},

	createRepository:
	function createRepository(repoPath, isBare){
		// Create a new repository in a clean directory
		return fse.remove(repoPath)
		.then(function() {
			return fse.ensureDir(repoPath);
		})
		.then(function() {
			var bare = typeof isBare !== "undefined" ? isBare : 0;
			return NodeGit.Repository.init(repoPath, bare);
		});
	},

	// Expects empty repo
	setupBranches:
	function setupBranches(repository, checkoutOurs) {
		var repoWorkDir = repository.workdir();

		var ourBranchName = "ours";
		var theirBranchName = "theirs";

		var baseFileName = "baseNewFile.txt";
		var ourFileName = "ourNewFile.txt";
		var theirFileName = "theirNewFile.txt";

		var baseFileContent = "How do you feel about Toll Roads?";
		var ourFileContent = "I like Toll Roads. I have an EZ-Pass!";
		var theirFileContent = "I'm skeptical about Toll Roads";

		var ourSignature = NodeGit.Signature.create
				("Ron Paul", "RonPaul@TollRoadsRBest.info", 123456789, 60);
		var theirSignature = NodeGit.Signature.create
				("Greg Abbott", "Gregggg@IllTollYourFace.us", 123456789, 60);

		var initialCommit;
		var ourBranch;
		var theirBranch;

		var ret = {
			ourBranchName: ourBranchName,
			theirBranchName: theirBranchName,

			ourSignature: ourSignature,
			theirSignature: theirSignature,

			ourFileName: ourFileName,
			theirFileName: theirFileName,

			ourFileContent: ourFileContent,
			theirFileContent: theirFileContent
		};

		return Promise.all([
			fse.writeFile(path.join(repoWorkDir, baseFileName),
										baseFileContent),
			fse.writeFile(path.join(repoWorkDir, ourFileName),
										ourFileContent),
			fse.writeFile(path.join(repoWorkDir, theirFileName),
										theirFileContent)
		])
			.then(function() {
				return RepositorySetup.addFileToIndex(repository, baseFileName);
			})
			.then(function(oid) {
				assert.equal(oid.toString(),
					"b5cdc109d437c4541a13fb7509116b5f03d5039a");

				return repository.createCommit(
					"HEAD", ourSignature, ourSignature,
					"initial commit", oid, []);
			})
			.then(function(commitOid) {
				assert.equal(commitOid.toString(),
					"be03abdf0353d05924c53bebeb0e5bb129cda44a");

				return repository.getCommit(commitOid);
			})
			.then(function(commit) {
				ret.initialCommit = initialCommit = commit;

				return Promise.all([
					repository.createBranch(ourBranchName, initialCommit),
					repository.createBranch(theirBranchName, initialCommit)
				]);
			})
			.then(function(branches) {
				assert(branches[0]);
				assert(branches[1]);

				ret.ourBranch = ourBranch = branches[0];
				ret.theirBranch = theirBranch = branches[1];

				return RepositorySetup.addFileToIndex(repository, ourFileName);
			})
			.then(function(oid) {
				assert.equal(oid.toString(),
					"77867fc0bfeb3f80ab18a78c8d53aa3a06207047");

				return repository.createCommit(ourBranch.name(), ourSignature,
					ourSignature, "we made a commit", oid, [initialCommit]);
			})
			.then(function(commitOid) {
				return repository.getCommit(commitOid);
			})
			.then(function(commit) {
				ret.ourCommit = commit;
				return NodeGit.Reset.default(
						repository, initialCommit, ourFileName);
			})
			.then(function() {
				return RepositorySetup.addFileToIndex(
						repository, theirFileName);
			})
			.then(function(oid) {
				assert.equal(oid.toString(),
					"be5f0fd38a39a67135ad68921c93cd5c17fefb3d");

				return repository.createCommit(
					theirBranch.name(), theirSignature,
					theirSignature, "they made a commit", oid, [initialCommit]);
			})
			.then(function(commitOid) {
				return repository.getCommit(commitOid);
			})
			.then(function(commit) {
				ret.theirCommit = commit;
				return NodeGit.Reset.default(
					repository, initialCommit, theirFileName);
			})
			.then(function() {
				return Promise.all([
					fse.remove(path.join(repoWorkDir, ourFileName)),
					fse.remove(path.join(repoWorkDir, theirFileName))
				]);
			})
			.then(function() {
				if (checkoutOurs) {
					var opts = {
						checkoutStrategy: NodeGit.Checkout.STRATEGY.FORCE
					};

					return repository.checkoutBranch(ourBranchName, opts);
				}
			})
			.then(function() {
				return ret;
			});
	}
};

module.exports = RepositorySetup;

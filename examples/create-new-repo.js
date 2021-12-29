const nodegit = require("../");
const path = require("path");
const fs = require("fs");
const fileName = "newfile.txt";
const fileContent = "hello world";
const repoDir = "../newRepo";


(async () => {
  await fs.promises.mkdir(path.resolve(__dirname, repoDir), {
    recursive: true,
  });
  const repo = await nodegit.Repository.init(path.resolve(__dirname, repoDir), 0);
  await fs.promises.writeFile(path.join(repo.workdir(), fileName), fileContent);
  const index = await repo.refreshIndex();
  await index.addByPath(fileName);
  await index.write();
  
  const oid = await index.writeTree();
  
  const author = nodegit.Signature.now("Scott Chacon",
    "schacon@gmail.com");
  const committer = nodegit.Signature.now("Scott A Chacon",
    "scott@github.com");

  // Since we're creating an initial commit, it has no parents. Note that unlike
  // normal we don't get the head either, because there isn't one yet.
  const commitId = await repo.createCommit("HEAD", author, committer, "message", oid, []);
  console.log("New Commit: ", commitId);
})();



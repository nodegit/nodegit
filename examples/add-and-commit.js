const nodegit = require("../");
const path = require("path");
const fs = require("fs");
const fileName = "newfile.txt";
const fileContent = "hello world";
const directoryName = "salad/toast/strangerinastrangeland/theresnowaythisexists";

/**
 * This example creates a certain file `newfile.txt`, adds it to the git
 * index and commits it to head. Similar to a `git add newfile.txt`
 * followed by a `git commit`
**/


(async () => {
  const repo = await nodegit.Repository.open(path.resolve(__dirname, "../.git"));
  
  await fs.promises.mkdir(path.join(repo.workdir(), directoryName), {
    recursive: true,
  });
  
  await fs.promises.writeFile(path.join(repo.workdir(), fileName), fileContent);
  await fs.promises.writeFile(
    path.join(repo.workdir(), directoryName, fileName),
    fileContent
  );
  
  const index = await repo.refreshIndex();
  
  // this file is in the root of the directory and doesn't need a full path
  await index.addByPath(fileName);
  // this file is in a subdirectory and can use a relative path
  await index.addByPath(path.posix.join(directoryName, fileName));
  // this will write both files to the index
  await index.write();
  
  const oid = await index.writeTree();
  
  const parent = await repo.getHeadCommit();
  const author = nodegit.Signature.now("Scott Chacon",
    "schacon@gmail.com");
  const committer = nodegit.Signature.now("Scott A Chacon",
    "scott@github.com");

  const commitId = await repo.createCommit("HEAD", author, committer, "message", oid, [parent]);

  console.log("New Commit: ", commitId);
})();

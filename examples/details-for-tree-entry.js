const nodegit = require("../");
const path = require("path");

/**
 * This shows how to get details from a tree entry or a blob
**/

(async () => {
  const repo = await nodegit.Repository.open(path.resolve(__dirname, "../.git"));
  const tree = await repo.getTree("e1b0c7ea57bfc5e30ec279402a98168a27838ac9");
  const treeEntry = tree.entryByIndex(0);
  
  // Tree entry doesn't have any data associated with the actual entry
  // To get that we need to get the index entry that this points to
  const index = await repo.refreshIndex();
  const indexEntry = index.getByPath(treeEntry.path());

  // With the index entry we can now view the details for the tree entry
  console.log("Entry path: " + indexEntry.path);
  console.log("Entry time in seconds: " + indexEntry.mtime.seconds());
  console.log("Entry oid: " + indexEntry.id.toString());
  console.log("Entry size: " + indexEntry.fileSize);
  
  console.log("Done!");
})();


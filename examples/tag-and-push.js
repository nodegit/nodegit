const nodegit = require("../");
const path = require("path");
const promisify = require("promisify-node");
const fse = promisify(require("fs-extra"));
const fileName = "newfile.txt";
const fileContent = "hello world";
const directoryName = "salad/toast/strangerinastrangeland/theresnowaythisexists";
fse.ensureDir = promisify(fse.ensureDir);

const commitTagAndPush = async () => {
  const repository = await Git.Repository.open(path.resolve(__dirname, "../.git"));
  await fse.ensureDir(path.join(repo.workdir(), directoryName));
  await fse.writeFile(
    path.join(repo.workdir(), directoryName, fileName),
    fileContent
  );

  const index = await repository.refreshIndex();
  await index.addAll();
  await index.write();
  const oid = await index.writeTree();
  const HEAD = await Git.Reference.nameToId(repository, 'HEAD');
  const parent = await repository.getCommit(HEAD);

  const author = Git.Signature.create('Scott Chacon', 'scott@github.com', 123456789, 0);
  const committer = Git.Signature.create('Scott Chacon', 'scott@github.com', 123456789, 0);
  const commit = await repository.createCommit('HEAD', author, committer, message, oid, [parent]);

  const tag = 'v1.0.15';
  const message = 'v1.0.15 is the very best version';

  await repository.createTag(commit, tag, message);
  const opts = {
    callbacks: {
      credentials: function(url, userName) {
        return nodegit.Cred.sshKeyFromAgent(userName);
      }
    }
  };
  const remote = await Git.Remote.lookup(repository, 'origin');
  return await remote.push([
    'refs/heads/master:refs/heads/master',
    `refs/tags/${name}:refs/tags/${name}`,
  ], opts);
};

commitTagAndPush();

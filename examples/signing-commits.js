const nodegit = require('../');
const path = require('path');
const fs = require('fs-extra');
const openpgp = require('openpgp');

const fileName = 'newfile.txt';
const fileContent = 'hello world';
const directoryName = 'salad/toast/strangerinastrangeland/theresnowaythisexists';
/**
 * This example creates a certain file `newfile.txt`, adds it to the git
 * index and commits with GOG signaature it to head. Similar to a `git add newfile.txt`
 * followed by a `git commit -s`
 *
 * This example uses `openpgp` library (https://github.com/openpgpjs/openpgpjs)
 * to read the key and encrypt the commit message. This involves two functions:
 * - the `decryptGpg` function which is responsible for decrypting the GPG key
 * - the `onSignature` function which is called during commit operation
 *
 * The `onSignature` function is the callback function passed to the
 * `createCommitWithSignature()` function of Repository instance.
 *
 * This example uses dummy GPG key. Learn how to generate a GPG key on HitHub
 * help pages: https://help.github.com/en/github/authenticating-to-github/generating-a-new-gpg-key
**/

/**
 * Decrypts GPG key to be used to sign commits.
 * @return {Promise}
 */
async function decryptGpg() {
  const key = path.join(__dirname, 'gpg-keys', 'secret-key-489B1963.asc');
  const pass = 'nodegit';
  const buff = await fs.readFile(key);
  const armored = await openpgp.key.readArmored(buff);
  const keyObj = armored.keys[0];
  const decrypted = keyObj.decrypt(pass);
  if (decrypted) {
    return keyObj;
  }
}

/**
 * Callback for GPG signature when signing the commit.
 * @param {String} tosign A string to sign.
 * @return {Promise}
 */
async function onSignature(tosign) {
  const privateKeyResult = await decryptGpg();
  if (!privateKeyResult) {
    throw new Error('GPG key decoding error.');
  }
  const buf = new Uint8Array(tosign.length);
  for (let i = 0; i < tosign.length; i++) {
    buf[i] = tosign.charCodeAt(i);
  }
  const options = {
    message: openpgp.message.fromBinary(buf),
    privateKeys: [privateKeyResult],
    detached: true
  };
  const signed = await openpgp.sign(options);
  return {
    code: nodegit.Git.Error.CODE.OK,
    field: 'gpgsig',
    signedData: signed.signature
  };
}

/**
 * Creates files in the local filesystem. This files will be added to the
 * repository.
 * @param {Repository} repo
 * @return {Promise} A promise resolved when files are created
 */
async function addFiles(repo) {
  await fs.ensureDir(path.join(repo.workdir(), directoryName));
  await fs.writeFile(path.join(repo.workdir(), fileName), fileContent);
  await fs.writeFile(
    path.join(repo.workdir(), directoryName, fileName),
    fileContent
  );
}
/**
 * Adds files by path and writes the state to the index.
 * @param {Repository} repo
 * @return {Promise} A promise resolved to an Oid
 */
async function addAndWrite(repo) {
  const index = await repo.refreshIndex();
  await index.addByPath(fileName);
  await index.addByPath(path.posix.join(directoryName, fileName));
  await index.write();
  return await index.writeTree();
}
/**
 * Commits changes using GPG key.
 * @param {Repository} repo
 * @param {Oid} oid Write Oid
 * @return {Promise} Resolved when the commit is ready.
 */
async function commitSigned(repo, oid) {
  const head = await nodegit.Reference.nameToId(repo, 'HEAD');
  const parent = await repo.getCommit(head);
  const author = nodegit.Signature.now('Pawel Psztyc', 'jarrodek@gmail.com');
  const committer = nodegit.Signature.now('Pawel Psztyc', 'jarrodek@gmail.com');
  return await repo.createCommitWithSignature(
    'HEAD',
    author,
    committer,
    'message',
    oid,
    [parent],
    onSignature
  );
}

let repo;
nodegit.Repository.open(path.resolve(__dirname, '../.git'))
.then((result) => {
  repo = result;
  return addFiles(repo);
})
.then(() => addAndWrite(repo))
.then((oid) => commitSigned(repo, oid))
.then((commitId) => {
  console.log(`New commit: ${commitId}`);
});

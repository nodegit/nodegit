const eslint = require("gulp-eslint");
const reporter = require("gulp-reporter");
const {
  Repository,
  Reference,
  Diff,
  Blob,
  Merge,
} = require("../");
const through = require("through2");
const Vinyl = require("vinyl");
const debug = require("gulp-debug");

function getRepository(){
  return Repository.open(process.env.GIT_DIR || ".git");
}

function cached() {
  return getRepository().then(repository => (
    Promise.all([
      Reference.nameToId(repository, "HEAD").then(head => (
        repository.getCommit(head)
      )).then(commit => (
        commit.getTree()
      )),
      repository.index(),
    ]).then(([old_tree, index]) => (
      Diff.treeToIndex(repository, old_tree, index).then(diff => (
        diff.patches()
      )).then(arrayConvenientPatch => (
        arrayConvenientPatch.map(convenientPatch => ({
          repository: repository,
          index: index,
          convenientPatch: convenientPatch,
          indexEntry: index.getByPath(convenientPatch.newFile().path()),
        }))
      ))
    ))
  ));
}

function diff() {
  return getRepository().then(repository => (
    Promise.all([
      repository.getMasterCommit().catch(()=>(
        repository.getBranchCommit("origin/master")
      )),
      repository.getHeadCommit(),
      repository.index(),
    ]).then(([masterCommit, headCommit, index]) => (
      Merge.base(repository, masterCommit, headCommit).then(oid => (
        repository.getCommit(oid)
      )).then(commit => (
        commit.getTree()
      )).then(old_tree => (
        Diff.treeToWorkdirWithIndex(repository, old_tree)
      )).then(diff => (
        diff.patches()
      )).then(arrayConvenientPatch => (
        arrayConvenientPatch.filter(convenientPatch => (
          !convenientPatch.isDeleted()
        )).map(convenientPatch => ({
          repository: repository,
          index: index,
          convenientPatch: convenientPatch,
          indexEntry: index.getByPath(convenientPatch.newFile().path()),
        }))
      ))
    ))
  ));
}

function readBlob(matcher) {
  return through.obj(function (file, enc, callback) {
    const {
      repository,
      indexEntry,
    } = file.git;
    Blob.lookup(repository, indexEntry.id).then(function(blob) {
      if(!blob.isBinary() && matcher.test(file.path)) {
        file.git.content = blob.content();
        file.contents = file.git.content;
        callback(null, file);
      } else {
        callback();
      } 
    }).catch(callback);
  });
}

function updateIndex() {
  let index;
  return through.obj(function (file, enc, callback) {
    if(file.contents.equals(file.git.content)) {
      callback(null, file);
      return;
    } else {
      const {
        repository,
        indexEntry,
      } = file.git;

      index = file.git.index;
      
      indexEntry.id = Blob.createFromBuffer(
        repository,
        file.contents,
        Buffer.byteLength(file.contents)
      );

      index.add(indexEntry).then(() => (
        callback(null, file)
      )).catch(callback);
    }
  }, function (cb) {
    // flush function
    if(index) {
      index.write().then(cb);
    } else {
      cb();
    }
  });  
}

function toStream(fn) {
  var stream = through.obj();
  fn().then(files => {
    files.forEach(file => {
      stream.push(new Vinyl({
        base: process.cwd(),
        cwd: __dirname,
        path: file.indexEntry.path,
        git: file,
      }));
    });
    stream.end();
  }).catch(ex => {
    stream.emit("error", ex);
  });
  return stream;
}

toStream(process.env.GIT_AUTHOR_DATE ? cached : diff)
  .pipe(readBlob(/(jsx?|es\d*)$/))
  .pipe(debug({
    title: "eslint"
  }))
  .pipe(eslint({
    fix: process.argv.indexOf("--fix") > 0,
  }))
  .pipe(updateIndex())
  .pipe(reporter({
    // fail only for new error.
    expires: new Date("2017-8-2"),
    author: null,
  })).resume().on("error", (e) => {
    console.error(String(e));
    process.exitCode = -1;
  });

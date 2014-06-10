/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <nan.h>
#include <string.h>

#include "git2.h"

#include "../include/functions/copy.h"

#include "../include/tree.h"
#include "../include/repo.h"
#include "../include/oid.h"
#include "../include/tree_entry.h"
#include "../include/diff_list.h"
#include "../include/diff_options.h"
#include "../include/tree_builder.h"
#include "../include/index.h"

using namespace v8;
using namespace node;

GitTree::GitTree(git_tree *raw) {
  this->raw = raw;
}

GitTree::~GitTree() {
  git_tree_free(this->raw);
}

void GitTree::Initialize(Handle<v8::Object> target) {
  NanScope();

  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanNew<String>("Tree"));

  NODE_SET_PROTOTYPE_METHOD(tpl, "oid", Oid);
  NODE_SET_PROTOTYPE_METHOD(tpl, "size", Size);
  NODE_SET_PROTOTYPE_METHOD(tpl, "entryByName", EntryByName);
  NODE_SET_PROTOTYPE_METHOD(tpl, "entryByIndex", EntryByIndex);
  NODE_SET_PROTOTYPE_METHOD(tpl, "entryByOid", EntryByOid);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getEntry", GetEntry);
  NODE_SET_PROTOTYPE_METHOD(tpl, "builder", Builder);
  NODE_SET_PROTOTYPE_METHOD(tpl, "diffTree", DiffTree);
  NODE_SET_PROTOTYPE_METHOD(tpl, "diffIndex", DiffIndex);
  NODE_SET_PROTOTYPE_METHOD(tpl, "diffWorkDir", DiffWorkDir);


  Local<Function> _constructor_template = tpl->GetFunction();
  NanAssignPersistent(constructor_template, _constructor_template);
  target->Set(NanNew<String>("Tree"), _constructor_template);
}

NAN_METHOD(GitTree::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError("git_tree is required.");
  }
  GitTree* object = new GitTree(static_cast<git_tree *>(Handle<External>::Cast(args[0])->Value()));
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitTree::New(void *raw) {
  NanEscapableScope();
  Handle<Value> argv[1] = { NanNew<External>((void *)raw) };
  return NanEscapeScope(NanNew<Function>(GitTree::constructor_template)->NewInstance(1, argv));
}

git_tree *GitTree::GetValue() {
  return this->raw;
}


/**
 * @return {Oid} result
 */
NAN_METHOD(GitTree::Oid) {
  NanScope();
  

  const git_oid * result = git_tree_id(
    ObjectWrap::Unwrap<GitTree>(args.This())->GetValue()
  );

  Handle<Value> to;
    if (result != NULL) {
    result = (const git_oid * )git_oid_dup(result);
  }
  if (result != NULL) {
    to = GitOid::New((void *)result);
  } else {
    to = NanNull();
  }
  NanReturnValue(to);
}

/**
 * @return {Number} result
 */
NAN_METHOD(GitTree::Size) {
  NanScope();
  

  size_t result = git_tree_entrycount(
    ObjectWrap::Unwrap<GitTree>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = NanNew<Uint32>((uint32_t)result);
  NanReturnValue(to);
}

/**
 * @param {String} filename
 * @return {TreeEntry} result
 */
NAN_METHOD(GitTree::EntryByName) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError("String filename is required.");
  }

  const char * from_filename;
            String::Utf8Value filename(args[0]->ToString());
      from_filename = strdup(*filename);
      
  const git_tree_entry * result = git_tree_entry_byname(
    ObjectWrap::Unwrap<GitTree>(args.This())->GetValue()
    , from_filename
  );
  free((void *)from_filename);

  Handle<Value> to;
    if (result != NULL) {
    result = (const git_tree_entry * )git_tree_entry_dup(result);
  }
  if (result != NULL) {
    to = GitTreeEntry::New((void *)result);
  } else {
    to = NanNull();
  }
  NanReturnValue(to);
}

/**
 * @param {Number} idx
 * @return {TreeEntry} result
 */
NAN_METHOD(GitTree::EntryByIndex) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsUint32()) {
    return NanThrowError("Number idx is required.");
  }

  size_t from_idx;
            from_idx = (size_t)   args[0]->ToUint32()->Value();
      
  const git_tree_entry * result = git_tree_entry_byindex(
    ObjectWrap::Unwrap<GitTree>(args.This())->GetValue()
    , from_idx
  );

  Handle<Value> to;
    if (result != NULL) {
    result = (const git_tree_entry * )git_tree_entry_dup(result);
  }
  if (result != NULL) {
    to = GitTreeEntry::New((void *)result);
  } else {
    to = NanNull();
  }
  NanReturnValue(to);
}

/**
 * @param {Oid} oid
 * @return {TreeEntry} result
 */
NAN_METHOD(GitTree::EntryByOid) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return NanThrowError("Oid oid is required.");
  }

  const git_oid * from_oid;
            from_oid = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject())->GetValue();
      
  const git_tree_entry * result = git_tree_entry_byoid(
    ObjectWrap::Unwrap<GitTree>(args.This())->GetValue()
    , from_oid
  );

  Handle<Value> to;
    if (result != NULL) {
    result = (const git_tree_entry * )git_tree_entry_dup(result);
  }
  if (result != NULL) {
    to = GitTreeEntry::New((void *)result);
  } else {
    to = NanNull();
  }
  NanReturnValue(to);
}

#include "../include/functions/copy.h"

/**
 * @param {String} path
 * @param {TreeEntry} callback
 */
NAN_METHOD(GitTree::GetEntry) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError("String path is required.");
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  GetEntryBaton* baton = new GetEntryBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->root = ObjectWrap::Unwrap<GitTree>(args.This())->GetValue();
    const char * from_path;
            String::Utf8Value path(args[0]->ToString());
      from_path = strdup(*path);
          baton->path = from_path;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[1]));
  GetEntryWorker *worker = new GetEntryWorker(baton, callback);
  worker->SaveToPersistent("root", args.This());
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("path", args[0]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitTree::GetEntryWorker::Execute() {
  int result = git_tree_entry_bypath(
    &baton->out, 
    baton->root, 
    baton->path
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitTree::GetEntryWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->out != NULL) {
    to = GitTreeEntry::New((void *)baton->out);
  } else {
    to = NanNull();
  }
  Handle<Value> result = to;
    Handle<Value> argv[2] = {
      NanNull(),
      result
    };
    callback->Call(2, argv);
  } else {
    if (baton->error) {
      Handle<Value> argv[1] = {
        NanError(baton->error->message)
      };
      callback->Call(1, argv);
      if (baton->error->message)
        free((void *)baton->error->message);
      free((void *)baton->error);
    } else {
      callback->Call(0, NULL);
    }
      }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  free((void *)baton->path);
  delete baton;
}

/**
 * @return {TreeBuilder} out
 */
NAN_METHOD(GitTree::Builder) {
  NanScope();
  
  git_treebuilder * out = 0;

  int result = git_treebuilder_create(
    &out
    , ObjectWrap::Unwrap<GitTree>(args.This())->GetValue()
  );
  if (result != GIT_OK) {
    if (giterr_last()) {
      return NanThrowError(giterr_last()->message);
    } else {
      return NanThrowError("Unknown Error");
    }
  }

  Handle<Value> to;
    if (out != NULL) {
    to = GitTreeBuilder::New((void *)out);
  } else {
    to = NanNull();
  }
  NanReturnValue(to);
}

#include "../include/functions/copy.h"

/**
 * @param {Repository} repo
 * @param {Tree} new_tree
 * @param {DiffOptions} opts
 * @param {DiffList} callback
 */
NAN_METHOD(GitTree::DiffTree) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsObject()) {
    return NanThrowError("Repository repo is required.");
  }
  if (args.Length() == 1 || !args[1]->IsObject()) {
    return NanThrowError("Tree new_tree is required.");
  }

  if (args.Length() == 3 || !args[3]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  DiffTreeBaton* baton = new DiffTreeBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
    git_repository * from_repo;
            from_repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
          baton->repo = from_repo;
    baton->old_tree = ObjectWrap::Unwrap<GitTree>(args.This())->GetValue();
    git_tree * from_new_tree;
            from_new_tree = ObjectWrap::Unwrap<GitTree>(args[1]->ToObject())->GetValue();
          baton->new_tree = from_new_tree;
      const git_diff_options * from_opts;
      if (args[2]->IsObject()) {
            from_opts = ObjectWrap::Unwrap<GitDiffOptions>(args[2]->ToObject())->GetValue();
          } else {
      from_opts = 0;
    }
      baton->opts = from_opts;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[3]));
  DiffTreeWorker *worker = new DiffTreeWorker(baton, callback);
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("repo", args[0]->ToObject());
  worker->SaveToPersistent("old_tree", args.This());
  if (!args[1]->IsUndefined() && !args[1]->IsNull())
    worker->SaveToPersistent("new_tree", args[1]->ToObject());
  if (!args[2]->IsUndefined() && !args[2]->IsNull())
    worker->SaveToPersistent("opts", args[2]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitTree::DiffTreeWorker::Execute() {
  int result = git_diff_tree_to_tree(
    &baton->diff, 
    baton->repo, 
    baton->old_tree, 
    baton->new_tree, 
    baton->opts
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitTree::DiffTreeWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->diff != NULL) {
    to = GitDiffList::New((void *)baton->diff);
  } else {
    to = NanNull();
  }
  Handle<Value> result = to;
    Handle<Value> argv[2] = {
      NanNull(),
      result
    };
    callback->Call(2, argv);
  } else {
    if (baton->error) {
      Handle<Value> argv[1] = {
        NanError(baton->error->message)
      };
      callback->Call(1, argv);
      if (baton->error->message)
        free((void *)baton->error->message);
      free((void *)baton->error);
    } else {
      callback->Call(0, NULL);
    }
      }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {Repository} repo
 * @param {Index} index
 * @param {DiffOptions} opts
 * @param {DiffList} callback
 */
NAN_METHOD(GitTree::DiffIndex) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsObject()) {
    return NanThrowError("Repository repo is required.");
  }

  if (args.Length() == 3 || !args[3]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  DiffIndexBaton* baton = new DiffIndexBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
    git_repository * from_repo;
            from_repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
          baton->repo = from_repo;
    baton->old_tree = ObjectWrap::Unwrap<GitTree>(args.This())->GetValue();
    git_index * from_index;
      if (args[1]->IsObject()) {
            from_index = ObjectWrap::Unwrap<GitIndex>(args[1]->ToObject())->GetValue();
          } else {
      from_index = 0;
    }
      baton->index = from_index;
      const git_diff_options * from_opts;
      if (args[2]->IsObject()) {
            from_opts = ObjectWrap::Unwrap<GitDiffOptions>(args[2]->ToObject())->GetValue();
          } else {
      from_opts = 0;
    }
      baton->opts = from_opts;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[3]));
  DiffIndexWorker *worker = new DiffIndexWorker(baton, callback);
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("repo", args[0]->ToObject());
  worker->SaveToPersistent("old_tree", args.This());
  if (!args[1]->IsUndefined() && !args[1]->IsNull())
    worker->SaveToPersistent("index", args[1]->ToObject());
  if (!args[2]->IsUndefined() && !args[2]->IsNull())
    worker->SaveToPersistent("opts", args[2]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitTree::DiffIndexWorker::Execute() {
  int result = git_diff_tree_to_index(
    &baton->diff, 
    baton->repo, 
    baton->old_tree, 
    baton->index, 
    baton->opts
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitTree::DiffIndexWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->diff != NULL) {
    to = GitDiffList::New((void *)baton->diff);
  } else {
    to = NanNull();
  }
  Handle<Value> result = to;
    Handle<Value> argv[2] = {
      NanNull(),
      result
    };
    callback->Call(2, argv);
  } else {
    if (baton->error) {
      Handle<Value> argv[1] = {
        NanError(baton->error->message)
      };
      callback->Call(1, argv);
      if (baton->error->message)
        free((void *)baton->error->message);
      free((void *)baton->error);
    } else {
      callback->Call(0, NULL);
    }
      }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {Repository} repo
 * @param {DiffOptions} opts
 * @param {DiffList} callback
 */
NAN_METHOD(GitTree::DiffWorkDir) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsObject()) {
    return NanThrowError("Repository repo is required.");
  }

  if (args.Length() == 2 || !args[2]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  DiffWorkDirBaton* baton = new DiffWorkDirBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
    git_repository * from_repo;
            from_repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
          baton->repo = from_repo;
    baton->old_tree = ObjectWrap::Unwrap<GitTree>(args.This())->GetValue();
    const git_diff_options * from_opts;
      if (args[1]->IsObject()) {
            from_opts = ObjectWrap::Unwrap<GitDiffOptions>(args[1]->ToObject())->GetValue();
          } else {
      from_opts = 0;
    }
      baton->opts = from_opts;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[2]));
  DiffWorkDirWorker *worker = new DiffWorkDirWorker(baton, callback);
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("repo", args[0]->ToObject());
  worker->SaveToPersistent("old_tree", args.This());
  if (!args[1]->IsUndefined() && !args[1]->IsNull())
    worker->SaveToPersistent("opts", args[1]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitTree::DiffWorkDirWorker::Execute() {
  int result = git_diff_tree_to_workdir(
    &baton->diff, 
    baton->repo, 
    baton->old_tree, 
    baton->opts
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitTree::DiffWorkDirWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->diff != NULL) {
    to = GitDiffList::New((void *)baton->diff);
  } else {
    to = NanNull();
  }
  Handle<Value> result = to;
    Handle<Value> argv[2] = {
      NanNull(),
      result
    };
    callback->Call(2, argv);
  } else {
    if (baton->error) {
      Handle<Value> argv[1] = {
        NanError(baton->error->message)
      };
      callback->Call(1, argv);
      if (baton->error->message)
        free((void *)baton->error->message);
      free((void *)baton->error);
    } else {
      callback->Call(0, NULL);
    }
      }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  delete baton;
}

Persistent<Function> GitTree::constructor_template;

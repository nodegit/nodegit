/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
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
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Tree"));

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


  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Tree"), constructor_template);
}

Handle<Value> GitTree::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return ThrowException(Exception::Error(String::New("git_tree is required.")));
  }

  GitTree* object = new GitTree((git_tree *) External::Unwrap(args[0]));
  object->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> GitTree::New(void *raw) {
  HandleScope scope;
  Handle<Value> argv[1] = { External::New((void *)raw) };
  return scope.Close(GitTree::constructor_template->NewInstance(1, argv));
}

git_tree *GitTree::GetValue() {
  return this->raw;
}


/**
 * @return {Oid} result
 */
Handle<Value> GitTree::Oid(const Arguments& args) {
  HandleScope scope;
  

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
    to = Null();
  }
  return scope.Close(to);
}

/**
 * @return {Number} result
 */
Handle<Value> GitTree::Size(const Arguments& args) {
  HandleScope scope;
  

  size_t result = git_tree_entrycount(
    ObjectWrap::Unwrap<GitTree>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = Uint32::New(result);
  return scope.Close(to);
}

/**
 * @param {String} filename
 * @return {TreeEntry} result
 */
Handle<Value> GitTree::EntryByName(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String filename is required.")));
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
    to = Null();
  }
  return scope.Close(to);
}

/**
 * @param {Number} idx
 * @return {TreeEntry} result
 */
Handle<Value> GitTree::EntryByIndex(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsUint32()) {
    return ThrowException(Exception::Error(String::New("Number idx is required.")));
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
    to = Null();
  }
  return scope.Close(to);
}

/**
 * @param {Oid} oid
 * @return {TreeEntry} result
 */
Handle<Value> GitTree::EntryByOid(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid oid is required.")));
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
    to = Null();
  }
  return scope.Close(to);
}

#include "../include/functions/copy.h"

/**
 * @param {String} path
 * @param {TreeEntry} callback
 */
Handle<Value> GitTree::GetEntry(const Arguments& args) {
  HandleScope scope;
      if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String path is required.")));
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  GetEntryBaton* baton = new GetEntryBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->rootReference = Persistent<Value>::New(args.This());
  baton->root = ObjectWrap::Unwrap<GitTree>(args.This())->GetValue();
  baton->pathReference = Persistent<Value>::New(args[0]);
    const char * from_path;
            String::Utf8Value path(args[0]->ToString());
      from_path = strdup(*path);
          baton->path = from_path;
    baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, GetEntryWork, (uv_after_work_cb)GetEntryAfterWork);

  return Undefined();
}

void GitTree::GetEntryWork(uv_work_t *req) {
  GetEntryBaton *baton = static_cast<GetEntryBaton *>(req->data);
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

void GitTree::GetEntryAfterWork(uv_work_t *req) {
  HandleScope scope;
  GetEntryBaton *baton = static_cast<GetEntryBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->out != NULL) {
    to = GitTreeEntry::New((void *)baton->out);
  } else {
    to = Null();
  }
  Handle<Value> result = to;
    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      result
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
  } else {
    if (baton->error) {
      Handle<Value> argv[1] = {
        Exception::Error(String::New(baton->error->message))
      };
      baton->callback->Call(Context::GetCurrent()->Global(), 1, argv);
      if (baton->error->message)
        free((void *)baton->error->message);
      free((void *)baton->error);
    } else {
      baton->callback->Call(Context::GetCurrent()->Global(), 0, NULL);
    }
      }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  baton->rootReference.Dispose();
  baton->pathReference.Dispose();
  baton->callback.Dispose();
  free((void *)baton->path);
  delete baton;
}

/**
 * @return {TreeBuilder} out
 */
Handle<Value> GitTree::Builder(const Arguments& args) {
  HandleScope scope;
  
  git_treebuilder * out = 0;

  int result = git_treebuilder_create(
    &out
    , ObjectWrap::Unwrap<GitTree>(args.This())->GetValue()
  );
  if (result != GIT_OK) {
    if (giterr_last()) {
      return ThrowException(Exception::Error(String::New(giterr_last()->message)));
    } else {
      return ThrowException(Exception::Error(String::New("Unkown Error")));
    }
  }

  Handle<Value> to;
    if (out != NULL) {
    to = GitTreeBuilder::New((void *)out);
  } else {
    to = Null();
  }
  return scope.Close(to);
}

#include "../include/functions/copy.h"

/**
 * @param {Repository} repo
 * @param {Tree} new_tree
 * @param {DiffOptions} opts
 * @param {DiffList} callback
 */
Handle<Value> GitTree::DiffTree(const Arguments& args) {
  HandleScope scope;
      if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository repo is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Tree new_tree is required.")));
  }

  if (args.Length() == 3 || !args[3]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  DiffTreeBaton* baton = new DiffTreeBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->repoReference = Persistent<Value>::New(args[0]);
    git_repository * from_repo;
            from_repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
          baton->repo = from_repo;
    baton->old_treeReference = Persistent<Value>::New(args.This());
  baton->old_tree = ObjectWrap::Unwrap<GitTree>(args.This())->GetValue();
  baton->new_treeReference = Persistent<Value>::New(args[1]);
    git_tree * from_new_tree;
            from_new_tree = ObjectWrap::Unwrap<GitTree>(args[1]->ToObject())->GetValue();
          baton->new_tree = from_new_tree;
    baton->optsReference = Persistent<Value>::New(args[2]);
    const git_diff_options * from_opts;
      if (args[2]->IsObject()) {
            from_opts = ObjectWrap::Unwrap<GitDiffOptions>(args[2]->ToObject())->GetValue();
          } else {
      from_opts = 0;
    }
      baton->opts = from_opts;
    baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[3]));

  uv_queue_work(uv_default_loop(), &baton->request, DiffTreeWork, (uv_after_work_cb)DiffTreeAfterWork);

  return Undefined();
}

void GitTree::DiffTreeWork(uv_work_t *req) {
  DiffTreeBaton *baton = static_cast<DiffTreeBaton *>(req->data);
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

void GitTree::DiffTreeAfterWork(uv_work_t *req) {
  HandleScope scope;
  DiffTreeBaton *baton = static_cast<DiffTreeBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->diff != NULL) {
    to = GitDiffList::New((void *)baton->diff);
  } else {
    to = Null();
  }
  Handle<Value> result = to;
    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      result
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
  } else {
    if (baton->error) {
      Handle<Value> argv[1] = {
        Exception::Error(String::New(baton->error->message))
      };
      baton->callback->Call(Context::GetCurrent()->Global(), 1, argv);
      if (baton->error->message)
        free((void *)baton->error->message);
      free((void *)baton->error);
    } else {
      baton->callback->Call(Context::GetCurrent()->Global(), 0, NULL);
    }
      }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  baton->repoReference.Dispose();
  baton->old_treeReference.Dispose();
  baton->new_treeReference.Dispose();
  baton->optsReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {Repository} repo
 * @param {Index} index
 * @param {DiffOptions} opts
 * @param {DiffList} callback
 */
Handle<Value> GitTree::DiffIndex(const Arguments& args) {
  HandleScope scope;
      if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository repo is required.")));
  }

  if (args.Length() == 3 || !args[3]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  DiffIndexBaton* baton = new DiffIndexBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->repoReference = Persistent<Value>::New(args[0]);
    git_repository * from_repo;
            from_repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
          baton->repo = from_repo;
    baton->old_treeReference = Persistent<Value>::New(args.This());
  baton->old_tree = ObjectWrap::Unwrap<GitTree>(args.This())->GetValue();
  baton->indexReference = Persistent<Value>::New(args[1]);
    git_index * from_index;
      if (args[1]->IsObject()) {
            from_index = ObjectWrap::Unwrap<GitIndex>(args[1]->ToObject())->GetValue();
          } else {
      from_index = 0;
    }
      baton->index = from_index;
    baton->optsReference = Persistent<Value>::New(args[2]);
    const git_diff_options * from_opts;
      if (args[2]->IsObject()) {
            from_opts = ObjectWrap::Unwrap<GitDiffOptions>(args[2]->ToObject())->GetValue();
          } else {
      from_opts = 0;
    }
      baton->opts = from_opts;
    baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[3]));

  uv_queue_work(uv_default_loop(), &baton->request, DiffIndexWork, (uv_after_work_cb)DiffIndexAfterWork);

  return Undefined();
}

void GitTree::DiffIndexWork(uv_work_t *req) {
  DiffIndexBaton *baton = static_cast<DiffIndexBaton *>(req->data);
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

void GitTree::DiffIndexAfterWork(uv_work_t *req) {
  HandleScope scope;
  DiffIndexBaton *baton = static_cast<DiffIndexBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->diff != NULL) {
    to = GitDiffList::New((void *)baton->diff);
  } else {
    to = Null();
  }
  Handle<Value> result = to;
    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      result
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
  } else {
    if (baton->error) {
      Handle<Value> argv[1] = {
        Exception::Error(String::New(baton->error->message))
      };
      baton->callback->Call(Context::GetCurrent()->Global(), 1, argv);
      if (baton->error->message)
        free((void *)baton->error->message);
      free((void *)baton->error);
    } else {
      baton->callback->Call(Context::GetCurrent()->Global(), 0, NULL);
    }
      }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  baton->repoReference.Dispose();
  baton->old_treeReference.Dispose();
  baton->indexReference.Dispose();
  baton->optsReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {Repository} repo
 * @param {DiffOptions} opts
 * @param {DiffList} callback
 */
Handle<Value> GitTree::DiffWorkDir(const Arguments& args) {
  HandleScope scope;
      if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository repo is required.")));
  }

  if (args.Length() == 2 || !args[2]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  DiffWorkDirBaton* baton = new DiffWorkDirBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->repoReference = Persistent<Value>::New(args[0]);
    git_repository * from_repo;
            from_repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
          baton->repo = from_repo;
    baton->old_treeReference = Persistent<Value>::New(args.This());
  baton->old_tree = ObjectWrap::Unwrap<GitTree>(args.This())->GetValue();
  baton->optsReference = Persistent<Value>::New(args[1]);
    const git_diff_options * from_opts;
      if (args[1]->IsObject()) {
            from_opts = ObjectWrap::Unwrap<GitDiffOptions>(args[1]->ToObject())->GetValue();
          } else {
      from_opts = 0;
    }
      baton->opts = from_opts;
    baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[2]));

  uv_queue_work(uv_default_loop(), &baton->request, DiffWorkDirWork, (uv_after_work_cb)DiffWorkDirAfterWork);

  return Undefined();
}

void GitTree::DiffWorkDirWork(uv_work_t *req) {
  DiffWorkDirBaton *baton = static_cast<DiffWorkDirBaton *>(req->data);
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

void GitTree::DiffWorkDirAfterWork(uv_work_t *req) {
  HandleScope scope;
  DiffWorkDirBaton *baton = static_cast<DiffWorkDirBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->diff != NULL) {
    to = GitDiffList::New((void *)baton->diff);
  } else {
    to = Null();
  }
  Handle<Value> result = to;
    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      result
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
  } else {
    if (baton->error) {
      Handle<Value> argv[1] = {
        Exception::Error(String::New(baton->error->message))
      };
      baton->callback->Call(Context::GetCurrent()->Global(), 1, argv);
      if (baton->error->message)
        free((void *)baton->error->message);
      free((void *)baton->error);
    } else {
      baton->callback->Call(Context::GetCurrent()->Global(), 0, NULL);
    }
      }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  baton->repoReference.Dispose();
  baton->old_treeReference.Dispose();
  baton->optsReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

Persistent<Function> GitTree::constructor_template;

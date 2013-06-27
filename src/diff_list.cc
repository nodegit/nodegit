/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/diff_list.h"
#include "../include/diff_options.h"
#include "../include/diff_find_options.h"
#include "../include/repo.h"
#include "../include/tree.h"
#include "../include/index.h"
#include "../include/patch.h"
#include "../include/delta.h"

#include "../include/functions/utilities.h"
#include "../include/functions/string.h"

using namespace v8;
using namespace node;

GitDiffList::GitDiffList(git_diff_list *raw) {
  this->raw = raw;
}

GitDiffList::~GitDiffList() {
  git_diff_list_free(this->raw);
}

void GitDiffList::Initialize(Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("DiffList"));

  NODE_SET_METHOD(tpl, "treeToTree", TreeToTree);
  NODE_SET_METHOD(tpl, "treeToIndex", TreeToIndex);
  NODE_SET_METHOD(tpl, "indexToWorkdir", IndexToWorkdir);
  NODE_SET_METHOD(tpl, "treeToWorkdir", TreeToWorkdir);
  NODE_SET_PROTOTYPE_METHOD(tpl, "merge", Merge);
  NODE_SET_PROTOTYPE_METHOD(tpl, "findSimilar", FindSimilar);
  NODE_SET_PROTOTYPE_METHOD(tpl, "size", Size);
  NODE_SET_METHOD(tpl, "numDeltasOfType", NumDeltasOfType);
  NODE_SET_PROTOTYPE_METHOD(tpl, "patch", Patch);


  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("DiffList"), constructor_template);
}

Handle<Value> GitDiffList::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return ThrowException(Exception::Error(String::New("git_diff_list is required.")));
  }

  GitDiffList* object = new GitDiffList((git_diff_list *) External::Unwrap(args[0]));
  object->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> GitDiffList::New(void *raw) {
  HandleScope scope;
  Handle<Value> argv[1] = { External::New((void *)raw) };
  return scope.Close(GitDiffList::constructor_template->NewInstance(1, argv));
}

git_diff_list *GitDiffList::GetValue() {
  return this->raw;
}


Handle<Value> GitDiffList::TreeToTree(const Arguments& args) {
  HandleScope scope;
  
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository repo is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Tree old_tree is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Tree new_tree is required.")));
  }

  if (args.Length() == 4 || !args[4]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  TreeToTreeBaton* baton = new TreeToTreeBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->repoReference = Persistent<Value>::New(args[0]);
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
  baton->old_treeReference = Persistent<Value>::New(args[1]);
  baton->old_tree = ObjectWrap::Unwrap<GitTree>(args[1]->ToObject())->GetValue();
  baton->new_treeReference = Persistent<Value>::New(args[2]);
  baton->new_tree = ObjectWrap::Unwrap<GitTree>(args[2]->ToObject())->GetValue();
  baton->optsReference = Persistent<Value>::New(args[3]);
  if (args[3]->IsObject()) {
    baton->opts = ObjectWrap::Unwrap<GitDiffOptions>(args[3]->ToObject())->GetValue();
  } else {
    baton->opts = NULL;
  }
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[4]));

  uv_queue_work(uv_default_loop(), &baton->request, TreeToTreeWork, (uv_after_work_cb)TreeToTreeAfterWork);

  return Undefined();
}

void GitDiffList::TreeToTreeWork(uv_work_t *req) {
  TreeToTreeBaton *baton = static_cast<TreeToTreeBaton *>(req->data);
  int result = git_diff_tree_to_tree(
    &baton->diff, 
    baton->repo, 
    baton->old_tree, 
    baton->new_tree, 
    baton->opts
  );
  baton->error_code = result;
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitDiffList::TreeToTreeAfterWork(uv_work_t *req) {
  HandleScope scope;
  TreeToTreeBaton *baton = static_cast<TreeToTreeBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    to = GitDiffList::New((void *)baton->diff);
  Handle<Value> result = to;
    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      result
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
  } else if (baton->error) {
    Handle<Value> argv[1] = {
      GitError::WrapError(baton->error)
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 1, argv);
  } else {
    baton->callback->Call(Context::GetCurrent()->Global(), 0, NULL);
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

Handle<Value> GitDiffList::TreeToIndex(const Arguments& args) {
  HandleScope scope;
  
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository repo is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Tree old_tree is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Index index is required.")));
  }
  if (args.Length() == 3 || !args[3]->IsObject()) {
    return ThrowException(Exception::Error(String::New("DiffOptions opts is required.")));
  }

  if (args.Length() == 4 || !args[4]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  TreeToIndexBaton* baton = new TreeToIndexBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->repoReference = Persistent<Value>::New(args[0]);
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
  baton->old_treeReference = Persistent<Value>::New(args[1]);
  baton->old_tree = ObjectWrap::Unwrap<GitTree>(args[1]->ToObject())->GetValue();
  baton->indexReference = Persistent<Value>::New(args[2]);
  baton->index = ObjectWrap::Unwrap<GitIndex>(args[2]->ToObject())->GetValue();
  baton->optsReference = Persistent<Value>::New(args[3]);
  baton->opts = ObjectWrap::Unwrap<GitDiffOptions>(args[3]->ToObject())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[4]));

  uv_queue_work(uv_default_loop(), &baton->request, TreeToIndexWork, (uv_after_work_cb)TreeToIndexAfterWork);

  return Undefined();
}

void GitDiffList::TreeToIndexWork(uv_work_t *req) {
  TreeToIndexBaton *baton = static_cast<TreeToIndexBaton *>(req->data);
  int result = git_diff_tree_to_index(
    &baton->diff, 
    baton->repo, 
    baton->old_tree, 
    baton->index, 
    baton->opts
  );
  baton->error_code = result;
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitDiffList::TreeToIndexAfterWork(uv_work_t *req) {
  HandleScope scope;
  TreeToIndexBaton *baton = static_cast<TreeToIndexBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    to = GitDiffList::New((void *)baton->diff);
  Handle<Value> result = to;
    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      result
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
  } else if (baton->error) {
    Handle<Value> argv[1] = {
      GitError::WrapError(baton->error)
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 1, argv);
  } else {
    baton->callback->Call(Context::GetCurrent()->Global(), 0, NULL);
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

Handle<Value> GitDiffList::IndexToWorkdir(const Arguments& args) {
  HandleScope scope;
  
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository repo is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Index index is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsObject()) {
    return ThrowException(Exception::Error(String::New("DiffOptions opts is required.")));
  }

  if (args.Length() == 3 || !args[3]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  IndexToWorkdirBaton* baton = new IndexToWorkdirBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->repoReference = Persistent<Value>::New(args[0]);
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
  baton->indexReference = Persistent<Value>::New(args[1]);
  baton->index = ObjectWrap::Unwrap<GitIndex>(args[1]->ToObject())->GetValue();
  baton->optsReference = Persistent<Value>::New(args[2]);
  baton->opts = ObjectWrap::Unwrap<GitDiffOptions>(args[2]->ToObject())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[3]));

  uv_queue_work(uv_default_loop(), &baton->request, IndexToWorkdirWork, (uv_after_work_cb)IndexToWorkdirAfterWork);

  return Undefined();
}

void GitDiffList::IndexToWorkdirWork(uv_work_t *req) {
  IndexToWorkdirBaton *baton = static_cast<IndexToWorkdirBaton *>(req->data);
  int result = git_diff_index_to_workdir(
    &baton->diff, 
    baton->repo, 
    baton->index, 
    baton->opts
  );
  baton->error_code = result;
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitDiffList::IndexToWorkdirAfterWork(uv_work_t *req) {
  HandleScope scope;
  IndexToWorkdirBaton *baton = static_cast<IndexToWorkdirBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    to = GitDiffList::New((void *)baton->diff);
  Handle<Value> result = to;
    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      result
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
  } else if (baton->error) {
    Handle<Value> argv[1] = {
      GitError::WrapError(baton->error)
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 1, argv);
  } else {
    baton->callback->Call(Context::GetCurrent()->Global(), 0, NULL);
  }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  baton->repoReference.Dispose();
  baton->indexReference.Dispose();
  baton->optsReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

Handle<Value> GitDiffList::TreeToWorkdir(const Arguments& args) {
  HandleScope scope;
  
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository repo is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Tree old_tree is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsObject()) {
    return ThrowException(Exception::Error(String::New("DiffOptions opts is required.")));
  }

  if (args.Length() == 3 || !args[3]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  TreeToWorkdirBaton* baton = new TreeToWorkdirBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->repoReference = Persistent<Value>::New(args[0]);
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
  baton->old_treeReference = Persistent<Value>::New(args[1]);
  baton->old_tree = ObjectWrap::Unwrap<GitTree>(args[1]->ToObject())->GetValue();
  baton->optsReference = Persistent<Value>::New(args[2]);
  baton->opts = ObjectWrap::Unwrap<GitDiffOptions>(args[2]->ToObject())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[3]));

  uv_queue_work(uv_default_loop(), &baton->request, TreeToWorkdirWork, (uv_after_work_cb)TreeToWorkdirAfterWork);

  return Undefined();
}

void GitDiffList::TreeToWorkdirWork(uv_work_t *req) {
  TreeToWorkdirBaton *baton = static_cast<TreeToWorkdirBaton *>(req->data);
  int result = git_diff_tree_to_workdir(
    &baton->diff, 
    baton->repo, 
    baton->old_tree, 
    baton->opts
  );
  baton->error_code = result;
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitDiffList::TreeToWorkdirAfterWork(uv_work_t *req) {
  HandleScope scope;
  TreeToWorkdirBaton *baton = static_cast<TreeToWorkdirBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    to = GitDiffList::New((void *)baton->diff);
  Handle<Value> result = to;
    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      result
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
  } else if (baton->error) {
    Handle<Value> argv[1] = {
      GitError::WrapError(baton->error)
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 1, argv);
  } else {
    baton->callback->Call(Context::GetCurrent()->Global(), 0, NULL);
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

Handle<Value> GitDiffList::Merge(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("DiffList from is required.")));
  }


  int result = git_diff_merge(
    ObjectWrap::Unwrap<GitDiffList>(args.This())->GetValue()
    , ObjectWrap::Unwrap<GitDiffList>(args[0]->ToObject())->GetValue()
  );

  if (result != GIT_OK) {
    return ThrowException(GitError::WrapError(giterr_last()));
  }

  return Undefined();
}

Handle<Value> GitDiffList::FindSimilar(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("DiffFindOptions options is required.")));
  }


  int result = git_diff_find_similar(
    ObjectWrap::Unwrap<GitDiffList>(args.This())->GetValue()
    , ObjectWrap::Unwrap<GitDiffFindOptions>(args[0]->ToObject())->GetValue()
  );

  if (result != GIT_OK) {
    return ThrowException(GitError::WrapError(giterr_last()));
  }

  return Undefined();
}

Handle<Value> GitDiffList::Size(const Arguments& args) {
  HandleScope scope;
  

  size_t result = git_diff_num_deltas(
    ObjectWrap::Unwrap<GitDiffList>(args.This())->GetValue()
  );


  Handle<Value> to;
    to = Uint32::New(result);
  return scope.Close(to);
}

Handle<Value> GitDiffList::NumDeltasOfType(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("DiffList diff is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number type is required.")));
  }


  size_t result = git_diff_num_deltas_of_type(
    ObjectWrap::Unwrap<GitDiffList>(args[0]->ToObject())->GetValue()
    , (git_delta_t) args[1]->ToInt32()->Value()
  );


  Handle<Value> to;
    to = Uint32::New(result);
  return scope.Close(to);
}

Handle<Value> GitDiffList::Patch(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsUint32()) {
    return ThrowException(Exception::Error(String::New("Number idx is required.")));
  }

  git_diff_patch *patch_out = NULL;
  const git_diff_delta *delta_out = NULL;

  int result = git_diff_get_patch(
    &patch_out
    , &delta_out
    , ObjectWrap::Unwrap<GitDiffList>(args.This())->GetValue()
    , (size_t) args[0]->ToUint32()->Value()
  );

  if (result != GIT_OK) {
    return ThrowException(GitError::WrapError(giterr_last()));
  }

  Handle<Object> toReturn = Object::New();
  Handle<Value> to;
      to = GitPatch::New((void *)patch_out);
    toReturn->Set(String::NewSymbol("patch"), to);

      to = GitDelta::New((void *)delta_out);
    toReturn->Set(String::NewSymbol("delta"), to);

  return scope.Close(toReturn);
}

Persistent<Function> GitDiffList::constructor_template;

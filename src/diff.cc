/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/diff.h"

#include "../include/functions/utilities.h"
#include "../include/functions/string.h"

using namespace v8;
using namespace node;

GitDiff::GitDiff(git_diff_list *raw) {
  this->raw = raw;
}

GitDiff::~GitDiff() {
  git_diff_list_free(this->raw);
}

void GitDiff::Initialize(Handle<v8::Object> target) {
  NanScope();

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanSymbol("Diff"));

  NODE_SET_METHOD(tpl, "treeToTree", TreeToTree);
  NODE_SET_METHOD(tpl, "treeToIndex", TreeToIndex);
  NODE_SET_METHOD(tpl, "indexToWorkdir", IndexToWorkdir);
  NODE_SET_METHOD(tpl, "treeToWorkdir", TreeToWorkdir);
  NODE_SET_PROTOTYPE_METHOD(tpl, "merge", Merge);
  NODE_SET_PROTOTYPE_METHOD(tpl, "findSimilar", FindSimilar);
  NODE_SET_METHOD(tpl, "statusChar", StatusChar);
  NODE_SET_PROTOTYPE_METHOD(tpl, "numDeltas", NumDeltas);
  NODE_SET_METHOD(tpl, "numDeltasOfType", NumDeltasOfType);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getPatch", GetPatch);

  NanAssignPersistent(FunctionTemplate, constructor_template, tpl);
  target->Set(String::NewSymbol("Diff"), tpl->GetFunction());
}

Handle<Value> GitDiff::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return ThrowException(Exception::Error(String::New("git_diff_list is required.")));
  }

  GitDiff* object = new GitDiff((git_diff_list *) External::Unwrap(args[0]));
  object->Wrap(args.This());

  return scope.Close(args.This());
}

git_diff_list *GitDiff::GetValue() {
  return this->raw;
}


Handle<Value> GitDiff::TreeToTree(const Arguments& args) {
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
  if (args.Length() == 3 || !args[3]->IsObject()) {
    return ThrowException(Exception::Error(String::New("DiffOptions opts is required.")));
  }
  if (args.Length() == 4 || !args[4]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  TreeToTreeBaton* baton = new TreeToTreeBaton;
  baton->error = NULL;
  baton->request.data = baton;
  baton->repoReference = Persistent<Value>::New(args[0]);
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
  baton->old_treeReference = Persistent<Value>::New(args[1]);
  baton->old_tree = ObjectWrap::Unwrap<GitTree>(args[1]->ToObject())->GetValue();
  baton->new_treeReference = Persistent<Value>::New(args[2]);
  baton->new_tree = ObjectWrap::Unwrap<GitTree>(args[2]->ToObject())->GetValue();
  baton->optsReference = Persistent<Value>::New(args[3]);
  baton->opts = ObjectWrap::Unwrap<DiffOptions>(args[3]->ToObject())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[4]));

  uv_queue_work(uv_default_loop(), &baton->request, TreeToTreeWork, (uv_after_work_cb)TreeToTreeAfterWork);

  return Undefined();
}

void GitDiff::TreeToTreeWork(uv_work_t *req) {
  TreeToTreeBaton *baton = static_cast<TreeToTreeBaton *>(req->data);
  int diff = git_diff_tree_to_tree(
    &baton->diff, 
    baton->repo, 
    baton->old_tree, 
    baton->new_tree, 
    baton->opts
  );
  if (diff != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitDiff::TreeToTreeAfterWork(uv_work_t *req) {
  HandleScope scope;
  TreeToTreeBaton *baton = static_cast<TreeToTreeBaton *>(req->data);

  TryCatch try_catch;
  if (!baton->error) {
    Handle<Value> argv[1] = { External::New(baton->diff) };
    Handle<Object> diff = GitDiffList::constructor_template->NewInstance(1, argv);
    Handle<Value> argv2[2] = {
      Local<Value>::New(Null()),
      diff
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv2);
  } else {
    Handle<Value> argv2[1] = {
      GitError::WrapError(baton->error)
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 1, argv2);
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

Handle<Value> GitDiff::TreeToIndex(const Arguments& args) {
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
  baton->error = NULL;
  baton->request.data = baton;
  baton->repoReference = Persistent<Value>::New(args[0]);
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
  baton->old_treeReference = Persistent<Value>::New(args[1]);
  baton->old_tree = ObjectWrap::Unwrap<GitTree>(args[1]->ToObject())->GetValue();
  baton->indexReference = Persistent<Value>::New(args[2]);
  baton->index = ObjectWrap::Unwrap<GitIndex>(args[2]->ToObject())->GetValue();
  baton->optsReference = Persistent<Value>::New(args[3]);
  baton->opts = ObjectWrap::Unwrap<DiffOptions>(args[3]->ToObject())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[4]));

  uv_queue_work(uv_default_loop(), &baton->request, TreeToIndexWork, (uv_after_work_cb)TreeToIndexAfterWork);

  return Undefined();
}

void GitDiff::TreeToIndexWork(uv_work_t *req) {
  TreeToIndexBaton *baton = static_cast<TreeToIndexBaton *>(req->data);
  int diff = git_diff_tree_to_index(
    &baton->diff, 
    baton->repo, 
    baton->old_tree, 
    baton->index, 
    baton->opts
  );
  if (diff != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitDiff::TreeToIndexAfterWork(uv_work_t *req) {
  HandleScope scope;
  TreeToIndexBaton *baton = static_cast<TreeToIndexBaton *>(req->data);

  TryCatch try_catch;
  if (!baton->error) {
    Handle<Value> argv[1] = { External::New(baton->diff) };
    Handle<Object> diff = GitDiffList::constructor_template->NewInstance(1, argv);
    Handle<Value> argv2[2] = {
      Local<Value>::New(Null()),
      diff
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv2);
  } else {
    Handle<Value> argv2[1] = {
      GitError::WrapError(baton->error)
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 1, argv2);
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

Handle<Value> GitDiff::IndexToWorkdir(const Arguments& args) {
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
  baton->error = NULL;
  baton->request.data = baton;
  baton->repoReference = Persistent<Value>::New(args[0]);
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
  baton->indexReference = Persistent<Value>::New(args[1]);
  baton->index = ObjectWrap::Unwrap<GitIndex>(args[1]->ToObject())->GetValue();
  baton->optsReference = Persistent<Value>::New(args[2]);
  baton->opts = ObjectWrap::Unwrap<DiffOptions>(args[2]->ToObject())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[3]));

  uv_queue_work(uv_default_loop(), &baton->request, IndexToWorkdirWork, (uv_after_work_cb)IndexToWorkdirAfterWork);

  return Undefined();
}

void GitDiff::IndexToWorkdirWork(uv_work_t *req) {
  IndexToWorkdirBaton *baton = static_cast<IndexToWorkdirBaton *>(req->data);
  int diff = git_diff_index_to_workdir(
    &baton->diff, 
    baton->repo, 
    baton->index, 
    baton->opts
  );
  if (diff != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitDiff::IndexToWorkdirAfterWork(uv_work_t *req) {
  HandleScope scope;
  IndexToWorkdirBaton *baton = static_cast<IndexToWorkdirBaton *>(req->data);

  TryCatch try_catch;
  if (!baton->error) {
    Handle<Value> argv[1] = { External::New(baton->diff) };
    Handle<Object> diff = GitDiffList::constructor_template->NewInstance(1, argv);
    Handle<Value> argv2[2] = {
      Local<Value>::New(Null()),
      diff
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv2);
  } else {
    Handle<Value> argv2[1] = {
      GitError::WrapError(baton->error)
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 1, argv2);
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

Handle<Value> GitDiff::TreeToWorkdir(const Arguments& args) {
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
  baton->error = NULL;
  baton->request.data = baton;
  baton->repoReference = Persistent<Value>::New(args[0]);
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
  baton->old_treeReference = Persistent<Value>::New(args[1]);
  baton->old_tree = ObjectWrap::Unwrap<GitTree>(args[1]->ToObject())->GetValue();
  baton->optsReference = Persistent<Value>::New(args[2]);
  baton->opts = ObjectWrap::Unwrap<DiffOptions>(args[2]->ToObject())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[3]));

  uv_queue_work(uv_default_loop(), &baton->request, TreeToWorkdirWork, (uv_after_work_cb)TreeToWorkdirAfterWork);

  return Undefined();
}

void GitDiff::TreeToWorkdirWork(uv_work_t *req) {
  TreeToWorkdirBaton *baton = static_cast<TreeToWorkdirBaton *>(req->data);
  int diff = git_diff_tree_to_workdir(
    &baton->diff, 
    baton->repo, 
    baton->old_tree, 
    baton->opts
  );
  if (diff != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitDiff::TreeToWorkdirAfterWork(uv_work_t *req) {
  HandleScope scope;
  TreeToWorkdirBaton *baton = static_cast<TreeToWorkdirBaton *>(req->data);

  TryCatch try_catch;
  if (!baton->error) {
    Handle<Value> argv[1] = { External::New(baton->diff) };
    Handle<Object> diff = GitDiffList::constructor_template->NewInstance(1, argv);
    Handle<Value> argv2[2] = {
      Local<Value>::New(Null()),
      diff
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv2);
  } else {
    Handle<Value> argv2[1] = {
      GitError::WrapError(baton->error)
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 1, argv2);
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

Handle<Value> GitDiff::Merge(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("DiffList from is required.")));
  }

  int result = git_diff_merge(


    ObjectWrap::Unwrap<GitDiff>(args.This())->GetValue()
, 

    ObjectWrap::Unwrap<GitDiffList>(args[0]->ToObject())->GetValue()
  );

  if (result != GIT_OK) {
    return ThrowException(GitError::WrapError(giterr_last()));
  }

  return scope.Close(Int32::New(result));
}

Handle<Value> GitDiff::FindSimilar(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("DiffFindOptions options is required.")));
  }

  int result = git_diff_find_similar(


    ObjectWrap::Unwrap<GitDiff>(args.This())->GetValue()
, 

    ObjectWrap::Unwrap<DiffFindOptions>(args[0]->ToObject())->GetValue()
  );

  if (result != GIT_OK) {
    return ThrowException(GitError::WrapError(giterr_last()));
  }

  return scope.Close(Int32::New(result));
}

Handle<Value> GitDiff::StatusChar(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number status is required.")));
  }

  char result = git_diff_status_char(


  (git_delta_t) args[0]->ToInt32()->Value()
  );


  return scope.Close(String::New(result));
}

Handle<Value> GitDiff::NumDeltas(const Arguments& args) {
  HandleScope scope;

  size_t result = git_diff_num_deltas(


    ObjectWrap::Unwrap<GitDiff>(args.This())->GetValue()
  );


  return scope.Close(Uint32::New(result));
}

Handle<Value> GitDiff::NumDeltasOfType(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("DiffList diff is required.")));
  }

  if (args.Length() == 1 || !args[1]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number type is required.")));
  }

  size_t result = git_diff_num_deltas_of_type(


    ObjectWrap::Unwrap<GitDiffList>(args[0]->ToObject())->GetValue()
, 

  (git_delta_t) args[1]->ToInt32()->Value()
  );


  return scope.Close(Uint32::New(result));
}

Handle<Value> GitDiff::GetPatch(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("DiffDelta delta_out is required.")));
  }

  if (args.Length() == 1 || !args[1]->IsUint32()) {
    return ThrowException(Exception::Error(String::New("Number idx is required.")));
  }
  git_diff_patch * patch_out;

  int result = git_diff_get_patch(

&
    patch_out
, 
&
    ObjectWrap::Unwrap<DiffDelta>(args[0]->ToObject())->GetValue()
, 

    ObjectWrap::Unwrap<GitDiff>(args.This())->GetValue()
, 

  (size_t) args[1]->ToUint32()->Value()
  );

  if (result != GIT_OK) {
    return ThrowException(GitError::WrapError(giterr_last()));
  }

  // XXX need to copy object?
  Handle<Value> argv[1] = { External::New((void *)patch_out) };
  return scope.Close(DiffPatch::constructor_template->NewInstance(1, argv));
}


Persistent<Function> GitDiff::constructor_template;

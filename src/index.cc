/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/index.h"

#include "../include/functions/utilities.h"
#include "../include/functions/string.h"

using namespace v8;
using namespace node;

Index::Index(git_index *raw) {
  this->raw = raw;
}

Index::~Index() {
  git_index_free(this->raw);
}

void Index::Initialize(Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Index"));

  NODE_SET_METHOD(tpl, "open", Open);
  NODE_SET_METHOD(tpl, "new", New);
  NODE_SET_PROTOTYPE_METHOD(tpl, "owner", Owner);
  NODE_SET_PROTOTYPE_METHOD(tpl, "read", Read);
  NODE_SET_PROTOTYPE_METHOD(tpl, "write", Write);
  NODE_SET_PROTOTYPE_METHOD(tpl, "readTree", ReadTree);
  NODE_SET_PROTOTYPE_METHOD(tpl, "writeTree", WriteTree);
  NODE_SET_PROTOTYPE_METHOD(tpl, "entrycount", Entrycount);
  NODE_SET_PROTOTYPE_METHOD(tpl, "clear", Clear);
  NODE_SET_PROTOTYPE_METHOD(tpl, "remove", Remove);
  NODE_SET_PROTOTYPE_METHOD(tpl, "removeDirectory", RemoveDirectory);
  NODE_SET_PROTOTYPE_METHOD(tpl, "addByPath", AddBypath);
  NODE_SET_PROTOTYPE_METHOD(tpl, "removeByPath", RemoveBypath);
  NODE_SET_PROTOTYPE_METHOD(tpl, "find", Find);
  NODE_SET_PROTOTYPE_METHOD(tpl, "conflictRemove", ConflictRemove);
  NODE_SET_PROTOTYPE_METHOD(tpl, "conflictCleanup", ConflictCleanup);
  NODE_SET_PROTOTYPE_METHOD(tpl, "hasConflicts", HasConflicts);


  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Index"), constructor_template);
}

Handle<Value> Index::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return ThrowException(Exception::Error(String::New("git_index is required.")));
  }

  Index* object = new Index((git_index *) External::Unwrap(args[0]));
  object->Wrap(args.This());

  return scope.Close(args.This());
}

git_index *Index::GetValue() {
  return this->raw;
}


Handle<Value> Index::Open(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String index_path is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  OpenBaton* baton = new OpenBaton;
  baton->error = NULL;
  baton->request.data = baton;
  baton->index_pathReference = Persistent<Value>::New(args[0]);
  String::Utf8Value index_path(args[0]->ToString());
  baton->index_path = strdup(*index_path);
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, OpenWork, (uv_after_work_cb)OpenAfterWork);

  return Undefined();
}

void Index::OpenWork(uv_work_t *req) {
  OpenBaton *baton = static_cast<OpenBaton *>(req->data);
  int result = git_index_open(
    &baton->out, 
    baton->index_path
  );
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void Index::OpenAfterWork(uv_work_t *req) {
  HandleScope scope;
  OpenBaton *baton = static_cast<OpenBaton *>(req->data);

  TryCatch try_catch;
  if (!baton->error) {
    Handle<Value> argv[1] = { External::New(baton->out) };
    Handle<Object> out = Index::constructor_template->NewInstance(1, argv);
    Handle<Value> argv2[2] = {
      Local<Value>::New(Null()),
      out
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
  baton->index_pathReference.Dispose();
  baton->callback.Dispose();
  delete baton->index_path;
  delete baton;
}

Handle<Value> Index::New(const Arguments& args) {
  HandleScope scope;
  git_index * out;

  int result = git_index_new(
&
    out
  );

  if (result != GIT_OK) {
    return ThrowException(GitError::WrapError(giterr_last()));
  }
  // XXX need to copy object?
  Handle<Value> argv[1] = { External::New((void *)out) };
  return scope.Close(Index::constructor_template->NewInstance(1, argv));
}

Handle<Value> Index::Owner(const Arguments& args) {
  HandleScope scope;

  git_repository * result = git_index_owner(

    ObjectWrap::Unwrap<Index>(args.This())->GetValue()
  );

  // XXX need to copy object?
  Handle<Value> argv[1] = { External::New((void *)result) };
  return scope.Close(GitRepo::constructor_template->NewInstance(1, argv));
}

Handle<Value> Index::Read(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  ReadBaton* baton = new ReadBaton;
  baton->error = NULL;
  baton->request.data = baton;
  baton->indexReference = Persistent<Value>::New(args.This());
  baton->index = ObjectWrap::Unwrap<Index>(args.This())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, ReadWork, (uv_after_work_cb)ReadAfterWork);

  return Undefined();
}

void Index::ReadWork(uv_work_t *req) {
  ReadBaton *baton = static_cast<ReadBaton *>(req->data);
  int result = git_index_read(
    baton->index
  );
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void Index::ReadAfterWork(uv_work_t *req) {
  HandleScope scope;
  ReadBaton *baton = static_cast<ReadBaton *>(req->data);

  TryCatch try_catch;
  if (!baton->error) {

    Handle<Value> result = Local<Value>::New(Undefined());
    Handle<Value> argv2[2] = {
      Local<Value>::New(Null()),
      result
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
  baton->indexReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

Handle<Value> Index::Write(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  WriteBaton* baton = new WriteBaton;
  baton->error = NULL;
  baton->request.data = baton;
  baton->indexReference = Persistent<Value>::New(args.This());
  baton->index = ObjectWrap::Unwrap<Index>(args.This())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, WriteWork, (uv_after_work_cb)WriteAfterWork);

  return Undefined();
}

void Index::WriteWork(uv_work_t *req) {
  WriteBaton *baton = static_cast<WriteBaton *>(req->data);
  int result = git_index_write(
    baton->index
  );
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void Index::WriteAfterWork(uv_work_t *req) {
  HandleScope scope;
  WriteBaton *baton = static_cast<WriteBaton *>(req->data);

  TryCatch try_catch;
  if (!baton->error) {

    Handle<Value> result = Local<Value>::New(Undefined());
    Handle<Value> argv2[2] = {
      Local<Value>::New(Null()),
      result
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
  baton->indexReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

Handle<Value> Index::ReadTree(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Tree tree is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  ReadTreeBaton* baton = new ReadTreeBaton;
  baton->error = NULL;
  baton->request.data = baton;
  baton->indexReference = Persistent<Value>::New(args.This());
  baton->index = ObjectWrap::Unwrap<Index>(args.This())->GetValue();
  baton->treeReference = Persistent<Value>::New(args[0]);
  baton->tree = ObjectWrap::Unwrap<GitTree>(args[0]->ToObject())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, ReadTreeWork, (uv_after_work_cb)ReadTreeAfterWork);

  return Undefined();
}

void Index::ReadTreeWork(uv_work_t *req) {
  ReadTreeBaton *baton = static_cast<ReadTreeBaton *>(req->data);
  int result = git_index_read_tree(
    baton->index, 
    baton->tree
  );
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void Index::ReadTreeAfterWork(uv_work_t *req) {
  HandleScope scope;
  ReadTreeBaton *baton = static_cast<ReadTreeBaton *>(req->data);

  TryCatch try_catch;
  if (!baton->error) {

    Handle<Value> result = Local<Value>::New(Undefined());
    Handle<Value> argv2[2] = {
      Local<Value>::New(Null()),
      result
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
  baton->indexReference.Dispose();
  baton->treeReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

Handle<Value> Index::WriteTree(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  WriteTreeBaton* baton = new WriteTreeBaton;
  baton->error = NULL;
  baton->request.data = baton;
  baton->indexReference = Persistent<Value>::New(args.This());
  baton->index = ObjectWrap::Unwrap<Index>(args.This())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, WriteTreeWork, (uv_after_work_cb)WriteTreeAfterWork);

  return Undefined();
}

void Index::WriteTreeWork(uv_work_t *req) {
  WriteTreeBaton *baton = static_cast<WriteTreeBaton *>(req->data);
  int result = git_index_write_tree(
    baton->out, 
    baton->index
  );
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void Index::WriteTreeAfterWork(uv_work_t *req) {
  HandleScope scope;
  WriteTreeBaton *baton = static_cast<WriteTreeBaton *>(req->data);

  TryCatch try_catch;
  if (!baton->error) {
    Handle<Value> argv[1] = { External::New(baton->out) };
    Handle<Object> out = GitOid::constructor_template->NewInstance(1, argv);
    Handle<Value> argv2[2] = {
      Local<Value>::New(Null()),
      out
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
  baton->indexReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

Handle<Value> Index::Entrycount(const Arguments& args) {
  HandleScope scope;

  size_t result = git_index_entrycount(

    ObjectWrap::Unwrap<Index>(args.This())->GetValue()
  );

  // XXX need to copy object?
  Handle<Value> argv[1] = { External::New((void *)result) };
  return scope.Close(size_t::constructor_template->NewInstance(1, argv));
}

Handle<Value> Index::Clear(const Arguments& args) {
  HandleScope scope;

  void result = git_index_clear(

    ObjectWrap::Unwrap<Index>(args.This())->GetValue()
  );

  return Undefined();
}

Handle<Value> Index::Remove(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String path is required.")));
  }

  if (args.Length() == 1 || !args[1]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number stage is required.")));
  }

  int result = git_index_remove(

    ObjectWrap::Unwrap<Index>(args.This())->GetValue()
, 
    stringArgToString(args[0]->ToString()).c_str()
, 
  (int) args[1]->ToInt32()->Value()
  );

  if (result != GIT_OK) {
    return ThrowException(GitError::WrapError(giterr_last()));
  }
  return scope.Close(Int32::New(result));
}

Handle<Value> Index::RemoveDirectory(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String dir is required.")));
  }

  if (args.Length() == 1 || !args[1]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number stage is required.")));
  }

  int result = git_index_remove_directory(

    ObjectWrap::Unwrap<Index>(args.This())->GetValue()
, 
    stringArgToString(args[0]->ToString()).c_str()
, 
  (int) args[1]->ToInt32()->Value()
  );

  if (result != GIT_OK) {
    return ThrowException(GitError::WrapError(giterr_last()));
  }
  return scope.Close(Int32::New(result));
}

Handle<Value> Index::AddBypath(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String path is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  AddBypathBaton* baton = new AddBypathBaton;
  baton->error = NULL;
  baton->request.data = baton;
  baton->indexReference = Persistent<Value>::New(args.This());
  baton->index = ObjectWrap::Unwrap<Index>(args.This())->GetValue();
  baton->pathReference = Persistent<Value>::New(args[0]);
  String::Utf8Value path(args[0]->ToString());
  baton->path = strdup(*path);
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, AddBypathWork, (uv_after_work_cb)AddBypathAfterWork);

  return Undefined();
}

void Index::AddBypathWork(uv_work_t *req) {
  AddBypathBaton *baton = static_cast<AddBypathBaton *>(req->data);
  int result = git_index_add_bypath(
    baton->index, 
    baton->path
  );
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void Index::AddBypathAfterWork(uv_work_t *req) {
  HandleScope scope;
  AddBypathBaton *baton = static_cast<AddBypathBaton *>(req->data);

  TryCatch try_catch;
  if (!baton->error) {

    Handle<Value> result = Local<Value>::New(Undefined());
    Handle<Value> argv2[2] = {
      Local<Value>::New(Null()),
      result
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
  baton->indexReference.Dispose();
  baton->pathReference.Dispose();
  baton->callback.Dispose();
  delete baton->path;
  delete baton;
}

Handle<Value> Index::RemoveBypath(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String path is required.")));
  }

  int result = git_index_remove_bypath(

    ObjectWrap::Unwrap<Index>(args.This())->GetValue()
, 
    stringArgToString(args[0]->ToString()).c_str()
  );

  if (result != GIT_OK) {
    return ThrowException(GitError::WrapError(giterr_last()));
  }
  return scope.Close(Int32::New(result));
}

Handle<Value> Index::Find(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("size_t at_pos is required.")));
  }

  if (args.Length() == 1 || !args[1]->IsString()) {
    return ThrowException(Exception::Error(String::New("String path is required.")));
  }

  int result = git_index_find(

    ObjectWrap::Unwrap<size_t>(args[0]->ToObject())->GetValue()
, 
    ObjectWrap::Unwrap<Index>(args.This())->GetValue()
, 
    stringArgToString(args[1]->ToString()).c_str()
  );

  return scope.Close(Int32::New(result));
}

Handle<Value> Index::ConflictRemove(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String path is required.")));
  }

  int result = git_index_conflict_remove(

    ObjectWrap::Unwrap<Index>(args.This())->GetValue()
, 
    stringArgToString(args[0]->ToString()).c_str()
  );

  if (result != GIT_OK) {
    return ThrowException(GitError::WrapError(giterr_last()));
  }
  return scope.Close(Int32::New(result));
}

Handle<Value> Index::ConflictCleanup(const Arguments& args) {
  HandleScope scope;

  void result = git_index_conflict_cleanup(

    ObjectWrap::Unwrap<Index>(args.This())->GetValue()
  );

  return Undefined();
}

Handle<Value> Index::HasConflicts(const Arguments& args) {
  HandleScope scope;

  int result = git_index_has_conflicts(

    ObjectWrap::Unwrap<Index>(args.This())->GetValue()
  );

  return scope.Close(Int32::New(result));
}


Persistent<Function> Index::constructor_template;

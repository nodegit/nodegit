/*
 * Copyright 2013, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * @author Michael Robinson @codeofinterest <mike@pagesofinterest.net>
 *
 * Dual licensed under the MIT and GPL licenses.
 */

#include <v8.h>
#include <node.h>

#include "cvv8/v8-convert.hpp"

#include "git2.h"

#include "../include/repo.h"
#include "../include/blob.h"
#include "../include/tree.h"
#include "../include/oid.h"
#include "../include/tree_entry.h"
#include "../include/error.h"

#include "../include/functions/utilities.h"

using namespace v8;
using namespace node;
using namespace cvv8;

namespace cvv8 {
  template <>
  struct NativeToJS<git_filemode_t> : NativeToJS<int32_t> {};
}

void GitTreeEntry::Initialize(Handle<v8::Object> target) {
  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("TreeEntry"));

  NODE_SET_PROTOTYPE_METHOD(tpl, "name", Name);
  NODE_SET_PROTOTYPE_METHOD(tpl, "root", Root);
  NODE_SET_PROTOTYPE_METHOD(tpl, "fileMode", FileMode);
  NODE_SET_PROTOTYPE_METHOD(tpl, "oid", Oid);
  NODE_SET_PROTOTYPE_METHOD(tpl, "toBlob", ToBlob);

  // Add libgit2 file modes to entry object
  Local<Object> libgit2FileModes = Object::New();

  libgit2FileModes->Set(String::NewSymbol("GIT_FILEMODE_NEW"), CastToJS(GIT_FILEMODE_NEW), ReadOnly);
  libgit2FileModes->Set(String::NewSymbol("GIT_FILEMODE_TREE"), CastToJS(GIT_FILEMODE_TREE), ReadOnly);
  libgit2FileModes->Set(String::NewSymbol("GIT_FILEMODE_BLOB"), CastToJS(GIT_FILEMODE_BLOB), ReadOnly);
  libgit2FileModes->Set(String::NewSymbol("GIT_FILEMODE_BLOB_EXECUTABLE"), CastToJS(GIT_FILEMODE_BLOB_EXECUTABLE), ReadOnly);
  libgit2FileModes->Set(String::NewSymbol("GIT_FILEMODE_LINK"), CastToJS(GIT_FILEMODE_LINK), ReadOnly);
  libgit2FileModes->Set(String::NewSymbol("GIT_FILEMODE_COMMIT"), CastToJS(GIT_FILEMODE_COMMIT), ReadOnly);

  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  constructor_template->Set(String::NewSymbol("fileModes"), libgit2FileModes, ReadOnly);
  target->Set(String::NewSymbol("TreeEntry"), constructor_template);
}

git_tree_entry* GitTreeEntry::GetValue() {
  return this->entry;
}
void GitTreeEntry::SetValue(git_tree_entry* entry) {
  this->entry = entry;
}
void GitTreeEntry::SetRoot(std::string root) {
  this->root = root;
}
std::string GitTreeEntry::GetRoot() {
  return this->root;
}

Handle<Value> GitTreeEntry::New(const Arguments& args) {
  HandleScope scope;

  GitTreeEntry *entry = new GitTreeEntry();

  entry->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> GitTreeEntry::Root(const Arguments& args) {
  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  GitTreeEntry *entry = ObjectWrap::Unwrap<GitTreeEntry>(args.This());

  Handle<Value> argv[2] = {
    Local<Value>::New(Null()),
    String::New(entry->GetRoot().c_str())
  };

  TryCatch try_catch;
  Local<Function>::Cast(args[0])->Call(Context::GetCurrent()->Global(), 2, argv);
  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }

  return Undefined();
}

Handle<Value> GitTreeEntry::Name(const Arguments& args) {
  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  NameBaton *baton = new NameBaton;
  baton->request.data = baton;

  GitTreeEntry *treeEntry = ObjectWrap::Unwrap<GitTreeEntry>(args.This());
  baton->rawEntry = treeEntry->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, NameWork, (uv_after_work_cb)NameAfterWork);

  return Undefined();
}
void GitTreeEntry::NameWork(uv_work_t* req) {
  NameBaton *baton = static_cast<NameBaton *>(req->data);

  baton->name = git_tree_entry_name(baton->rawEntry);
}
void GitTreeEntry::NameAfterWork(uv_work_t* req) {
  HandleScope scope;
  NameBaton *baton = static_cast<NameBaton *>(req->data);

  Handle<Value> argv[2] = {
    Local<Value>::New(Null()),
    String::New(baton->name)
  };

  TryCatch try_catch;
  baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  delete req;
}

Handle<Value> GitTreeEntry::FileMode(const Arguments& args) {
  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  FileModeBaton *baton = new FileModeBaton;
  baton->request.data = baton;
  baton->rawEntry = ObjectWrap::Unwrap<GitTreeEntry>(args.This())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, FileModeWork, (uv_after_work_cb)FileModeAfterWork);

  return Undefined();
}
void GitTreeEntry::FileModeWork(uv_work_t* req) {
  FileModeBaton *baton = static_cast<FileModeBaton *>(req->data);

  baton->fileMode = git_tree_entry_filemode(baton->rawEntry);
}
void GitTreeEntry::FileModeAfterWork(uv_work_t* req) {
  HandleScope scope;
  FileModeBaton *baton = static_cast<FileModeBaton *>(req->data);

  Handle<Value> argv[2] = {
    Local<Value>::New(Null()),
    Integer::New(baton->fileMode)
  };

  TryCatch try_catch;
  baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  delete req;
}

Handle<Value> GitTreeEntry::Oid(const Arguments& args) {
  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  OidBaton* baton = new OidBaton;
  baton->request.data = baton;
  baton->rawEntry = ObjectWrap::Unwrap<GitTreeEntry>(args.This())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, OidWork, (uv_after_work_cb)OidAfterWork);

  return Undefined();
}
void GitTreeEntry::OidWork(uv_work_t* req) {
  OidBaton *baton = static_cast<OidBaton *>(req->data);

  baton->rawOid = git_tree_entry_id(const_cast<git_tree_entry *>(baton->rawEntry));
}
void GitTreeEntry::OidAfterWork(uv_work_t* req) {
  HandleScope scope;
  OidBaton *baton = static_cast<OidBaton *>(req->data);
  Handle<Value> argv[1] = { External::New((void *) baton->rawOid) };
  Handle<Object> oid = GitOid::constructor_template->NewInstance(1, argv);

  Handle<Value> argv2[2] = {
    Local<Value>::New(Null()),
    oid
  };

  TryCatch try_catch;
  baton->callback->Call(Context::GetCurrent()->Global(), 2, argv2);
  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  delete req;
}

Handle<Value> GitTreeEntry::ToBlob(const Arguments& args) {
  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository is required and must be an Object.")));
  }

  if(args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  ToBlobBaton* baton = new ToBlobBaton;
  baton->request.data = baton;
  baton->error = NULL;
  baton->rawRepo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
  baton->rawEntry = ObjectWrap::Unwrap<GitTreeEntry>(args.This())->GetValue();
  baton->rawBlob = NULL;
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, ToBlobWork, (uv_after_work_cb)ToBlobAfterWork);

  return Undefined();
}
void GitTreeEntry::ToBlobWork(uv_work_t *req) {
  ToBlobBaton* baton = static_cast<ToBlobBaton*>(req->data);

  int returnCode = git_tree_entry_to_object((git_object**)&baton->rawBlob, baton->rawRepo, baton->rawEntry);
  if (returnCode != GIT_OK) {
    baton->error = giterr_last();
  }
}
void GitTreeEntry::ToBlobAfterWork(uv_work_t *req) {
  HandleScope scope;
  ToBlobBaton* baton = static_cast<ToBlobBaton* >(req->data);

  if (success(baton->error, baton->callback)) {
    Handle<Object> blob = GitBlob::constructor_template->NewInstance();
    GitBlob *blobInstance = ObjectWrap::Unwrap<GitBlob>(blob);
    blobInstance->SetValue(baton->rawBlob);

    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      blob
    };

    TryCatch try_catch;
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
    if (try_catch.HasCaught()) {
      node::FatalException(try_catch);
    }
  }
  delete req;
}

Persistent<Function> GitTreeEntry::constructor_template;


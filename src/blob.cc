/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/blob.h"
#include "../include/repo.h"
#include "../include/oid.h"
#include "../include/wrapper.h"
#include "node_buffer.h"

#include "../include/functions/utilities.h"
#include "../include/functions/string.h"

using namespace v8;
using namespace node;

GitBlob::GitBlob(git_blob *raw) {
  this->raw = raw;
}

GitBlob::~GitBlob() {
  git_blob_free(this->raw);
}

void GitBlob::Initialize(Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Blob"));

  NODE_SET_METHOD(tpl, "lookup", Lookup);
  NODE_SET_PROTOTYPE_METHOD(tpl, "oid", Oid);
  NODE_SET_PROTOTYPE_METHOD(tpl, "content", Content);
  NODE_SET_PROTOTYPE_METHOD(tpl, "size", Size);
  NODE_SET_METHOD(tpl, "createFromFile", CreateFromFile);
  NODE_SET_METHOD(tpl, "createFromBuffer", CreateFromBuffer);
  NODE_SET_PROTOTYPE_METHOD(tpl, "isBinary", IsBinary);

  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Blob"), constructor_template);
}

Handle<Value> GitBlob::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return ThrowException(Exception::Error(String::New("git_blob is required.")));
  }

  GitBlob* object = new GitBlob((git_blob *) External::Unwrap(args[0]));
  object->Wrap(args.This());

  return scope.Close(args.This());
}

git_blob *GitBlob::GetValue() {
  return this->raw;
}


Handle<Value> GitBlob::Lookup(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  LookupBaton* baton = new LookupBaton;
  baton->error = NULL;
  baton->request.data = baton;

  // XXX FIXME potential GC issue: if the argument gets GCd, the destructor could null out this object.
  // Either ref the argument or copy?
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
  // XXX FIXME potential GC issue: if the argument gets GCd, the destructor could null out this object.
  // Either ref the argument or copy?
  baton->id = ObjectWrap::Unwrap<GitOid>(args[1]->ToObject())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[2]));

  uv_queue_work(uv_default_loop(), &baton->request, LookupWork, (uv_after_work_cb)LookupAfterWork);

  return Undefined();
}

void GitBlob::LookupWork(uv_work_t *req) {
  LookupBaton *baton = static_cast<LookupBaton *>(req->data);
  int result = git_blob_lookup(
    &baton->blob, 
    baton->repo, 
    baton->id
  );
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitBlob::LookupAfterWork(uv_work_t *req) {
  HandleScope scope;
  LookupBaton *baton = static_cast<LookupBaton *>(req->data);

  TryCatch try_catch;
  if (!baton->error) {

    Handle<Value> argv[1] = { External::New(baton->blob) };
    Handle<Object> object = GitBlob::constructor_template->NewInstance(1, argv);
    Handle<Value> argv2[2] = {
      Local<Value>::New(Null()),
      object
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

  baton->callback.Dispose();
  delete baton;
}

Handle<Value> GitBlob::Oid(const Arguments& args) {
  HandleScope scope;

  const git_oid * result = git_blob_id(
    ObjectWrap::Unwrap<GitBlob>(args.This())->GetValue()
  );

  // XXX need to copy object?
  Handle<Value> argv[1] = { External::New((void *)result) };
  return scope.Close(GitOid::constructor_template->NewInstance(1, argv));
}

Handle<Value> GitBlob::Content(const Arguments& args) {
  HandleScope scope;

  const void * result = git_blob_rawcontent(
    ObjectWrap::Unwrap<GitBlob>(args.This())->GetValue()
  );

  // XXX need to copy object?
  Handle<Value> argv[1] = { External::New((void *)result) };
  return scope.Close(Wrapper::constructor_template->NewInstance(1, argv));
}

Handle<Value> GitBlob::Size(const Arguments& args) {
  HandleScope scope;

  git_off_t result = git_blob_rawsize(
    ObjectWrap::Unwrap<GitBlob>(args.This())->GetValue()
  );

  return scope.Close(Number::New(result));
}

Handle<Value> GitBlob::CreateFromFile(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsString()) {
    return ThrowException(Exception::Error(String::New("String is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  CreateFromFileBaton* baton = new CreateFromFileBaton;
  baton->error = NULL;
  baton->request.data = baton;

  // XXX FIXME potential GC issue: if the argument gets GCd, the destructor could null out this object.
  // Either ref the argument or copy?
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
  String::Utf8Value path(args[1]->ToString());
  // XXX FIXME remove strdup and use std::string
  baton->path = strdup(*path);
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[2]));

  uv_queue_work(uv_default_loop(), &baton->request, CreateFromFileWork, (uv_after_work_cb)CreateFromFileAfterWork);

  return Undefined();
}

void GitBlob::CreateFromFileWork(uv_work_t *req) {
  CreateFromFileBaton *baton = static_cast<CreateFromFileBaton *>(req->data);
  int result = git_blob_create_fromdisk(
    baton->id, 
    baton->repo, 
    baton->path
  );
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitBlob::CreateFromFileAfterWork(uv_work_t *req) {
  HandleScope scope;
  CreateFromFileBaton *baton = static_cast<CreateFromFileBaton *>(req->data);

  TryCatch try_catch;
  if (!baton->error) {

    Handle<Value> argv[1] = { External::New(baton->id) };
    Handle<Object> object = GitOid::constructor_template->NewInstance(1, argv);
    // free((void *) baton->path);
    Handle<Value> argv2[2] = {
      Local<Value>::New(Null()),
      object
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

  baton->callback.Dispose();

  delete baton->path;
  delete baton;
}

Handle<Value> GitBlob::CreateFromBuffer(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Buffer is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsNumber()) {
    return ThrowException(Exception::Error(String::New("Number is required.")));
  }
  if (args.Length() == 3 || !args[3]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  CreateFromBufferBaton* baton = new CreateFromBufferBaton;
  baton->error = NULL;
  baton->request.data = baton;

  // XXX FIXME potential GC issue: if the argument gets GCd, the destructor could null out this object.
  // Either ref the argument or copy?
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
  baton->buffer = Buffer::Data(ObjectWrap::Unwrap<Buffer>(args[1]->ToObject()));
  baton->len = args[2]->ToNumber()->Value();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[3]));

  uv_queue_work(uv_default_loop(), &baton->request, CreateFromBufferWork, (uv_after_work_cb)CreateFromBufferAfterWork);

  return Undefined();
}

void GitBlob::CreateFromBufferWork(uv_work_t *req) {
  CreateFromBufferBaton *baton = static_cast<CreateFromBufferBaton *>(req->data);
  int result = git_blob_create_frombuffer(
    baton->oid, 
    baton->repo, 
    baton->buffer, 
    baton->len
  );
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitBlob::CreateFromBufferAfterWork(uv_work_t *req) {
  HandleScope scope;
  CreateFromBufferBaton *baton = static_cast<CreateFromBufferBaton *>(req->data);

  TryCatch try_catch;
  if (!baton->error) {

    Handle<Value> argv[1] = { External::New(baton->oid) };
    Handle<Object> object = GitOid::constructor_template->NewInstance(1, argv);
    Handle<Value> argv2[2] = {
      Local<Value>::New(Null()),
      object
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

  baton->callback.Dispose();
  delete baton;
}

Handle<Value> GitBlob::IsBinary(const Arguments& args) {
  HandleScope scope;

  int result = git_blob_is_binary(
    ObjectWrap::Unwrap<GitBlob>(args.This())->GetValue()
  );

  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }
  return scope.Close(Boolean::New(result));
}

Persistent<Function> GitBlob::constructor_template;

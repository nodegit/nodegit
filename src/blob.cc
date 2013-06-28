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

Handle<Value> GitBlob::New(void *raw) {
  HandleScope scope;
  Handle<Value> argv[1] = { External::New((void *)raw) };
  return scope.Close(GitBlob::constructor_template->NewInstance(1, argv));
}

git_blob *GitBlob::GetValue() {
  return this->raw;
}


Handle<Value> GitBlob::Oid(const Arguments& args) {
  HandleScope scope;
  

  const git_oid * result = git_blob_id(
    ObjectWrap::Unwrap<GitBlob>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = GitOid::New((void *)result);
  return scope.Close(to);
}

Handle<Value> GitBlob::Content(const Arguments& args) {
  HandleScope scope;
  

  const void * result = git_blob_rawcontent(
    ObjectWrap::Unwrap<GitBlob>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = Wrapper::New((void *)result);
  return scope.Close(to);
}

Handle<Value> GitBlob::Size(const Arguments& args) {
  HandleScope scope;
  

  git_off_t result = git_blob_rawsize(
    ObjectWrap::Unwrap<GitBlob>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = Number::New(result);
  return scope.Close(to);
}

Handle<Value> GitBlob::CreateFromFile(const Arguments& args) {
  HandleScope scope;
      if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository repo is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsString()) {
    return ThrowException(Exception::Error(String::New("String path is required.")));
  }

  if (args.Length() == 2 || !args[2]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  CreateFromFileBaton* baton = new CreateFromFileBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->repoReference = Persistent<Value>::New(args[0]);
    git_repository * from_repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
  baton->repo = from_repo;
  baton->pathReference = Persistent<Value>::New(args[1]);
    String::Utf8Value path(args[1]->ToString());
  const char * from_path = strdup(*path);
  baton->path = from_path;
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
  baton->error_code = result;
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitBlob::CreateFromFileAfterWork(uv_work_t *req) {
  HandleScope scope;
  CreateFromFileBaton *baton = static_cast<CreateFromFileBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    to = GitOid::New((void *)baton->id);
  Handle<Value> result = to;
    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      result
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
  } else if (baton->error) {
    Handle<Value> argv[1] = {
      Exception::Error(String::New(baton->error->message))
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 1, argv);
  } else {
    baton->callback->Call(Context::GetCurrent()->Global(), 0, NULL);
  }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  baton->repoReference.Dispose();
  baton->pathReference.Dispose();
  baton->callback.Dispose();
  delete baton->path;
  delete baton;
}

Handle<Value> GitBlob::CreateFromBuffer(const Arguments& args) {
  HandleScope scope;
      if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository repo is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Buffer buffer is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsNumber()) {
    return ThrowException(Exception::Error(String::New("Number len is required.")));
  }

  if (args.Length() == 3 || !args[3]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  CreateFromBufferBaton* baton = new CreateFromBufferBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->repoReference = Persistent<Value>::New(args[0]);
    git_repository * from_repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
  baton->repo = from_repo;
  baton->bufferReference = Persistent<Value>::New(args[1]);
    const void * from_buffer = Buffer::Data(ObjectWrap::Unwrap<Buffer>(args[1]->ToObject()));
  baton->buffer = from_buffer;
  baton->lenReference = Persistent<Value>::New(args[2]);
    size_t from_len = (size_t) args[2]->ToNumber()->Value();
  baton->len = from_len;
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
  baton->error_code = result;
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitBlob::CreateFromBufferAfterWork(uv_work_t *req) {
  HandleScope scope;
  CreateFromBufferBaton *baton = static_cast<CreateFromBufferBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    to = GitOid::New((void *)baton->oid);
  Handle<Value> result = to;
    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      result
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
  } else if (baton->error) {
    Handle<Value> argv[1] = {
      Exception::Error(String::New(baton->error->message))
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 1, argv);
  } else {
    baton->callback->Call(Context::GetCurrent()->Global(), 0, NULL);
  }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  baton->repoReference.Dispose();
  baton->bufferReference.Dispose();
  baton->lenReference.Dispose();
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

  return Undefined();
}

Persistent<Function> GitBlob::constructor_template;

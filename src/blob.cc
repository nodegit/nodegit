/*
 * Copyright 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * Dual licensed under the MIT and GPL licenses.
 */

#include <node_buffer.h>

#include "../vendor/libgit2/include/git2.h"

#include "../include/utils.h"
#include "../include/repo.h"
#include "../include/blob.h"

#include "../include/functions/utilities.h"

using namespace v8;
using namespace node;

void GitBlob::Initialize (Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Blob"));

  NODE_SET_PROTOTYPE_METHOD(tpl, "lookup", Lookup);
  NODE_SET_PROTOTYPE_METHOD(tpl, "rawContent", RawContent);
  NODE_SET_PROTOTYPE_METHOD(tpl, "close", Close);
  NODE_SET_PROTOTYPE_METHOD(tpl, "createFromFile", CreateFromFile);
  NODE_SET_PROTOTYPE_METHOD(tpl, "createFromBuffer", CreateFromBuffer);

  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Blob"), constructor_template);
}

git_blob* GitBlob::GetValue() {
  return this->blob;
}
void GitBlob::SetValue(git_blob* blob) {
  this->blob = blob;
}

void GitBlob::Close() {
  git_blob_free(this->blob);
}

int CreateFromFile(git_oid* oid, git_repository* repo, const char* path) {
  return git_blob_create_fromdisk(oid, repo, path);
}
int CreateFromBuffer(git_oid* oid, git_repository* repo, const void* buffer, size_t len) {
  return git_blob_create_frombuffer(oid, repo, buffer, len);
}

Handle<Value> GitBlob::New(const Arguments& args) {
  HandleScope scope;

  GitBlob* blob = new GitBlob();
  blob->Wrap(args.This());

  return scope.Close( args.This() );
}

Handle<Value> GitBlob::Lookup(const Arguments& args) {
  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repo is required and must be an Object.")));
  }

  if(args.Length() == 1 || !args[1]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid is required and must be an Object.")));
  }

  if(args.Length() == 2 || !args[2]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  LookupBaton* baton = new LookupBaton;
  baton->request.data = baton;
  baton->blob = ObjectWrap::Unwrap<GitBlob>(args.This());
  baton->blob->Ref();
  baton->rawBlob = baton->blob->GetValue();
  baton->rawRepo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
  baton->rawOid = ObjectWrap::Unwrap<GitOid>(args[1]->ToObject())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[2]));

  uv_queue_work(uv_default_loop(), &baton->request, LookupWork, (uv_after_work_cb)LookupAfterWork);

  return Undefined();
}
void GitBlob::LookupWork(uv_work_t* req) {
  LookupBaton* baton = static_cast<LookupBaton* >(req->data);
  int returnCode =  git_blob_lookup(&baton->rawBlob, baton->rawRepo, &baton->rawOid);
  if (returnCode != GIT_OK) {
    baton->error = giterr_last();
  }
}
void GitBlob::LookupAfterWork(uv_work_t* req) {
  HandleScope scope;
  LookupBaton* baton = static_cast<LookupBaton* >(req->data);

  if (success(baton->error, baton->callback)) {
    baton->blob->SetValue(baton->rawBlob);

    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      baton->blob->handle_
    };

    TryCatch try_catch;
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
    if (try_catch.HasCaught()) {
      node::FatalException(try_catch);
    }
  }
  delete req;
}

Handle<Value> GitBlob::RawContent(const Arguments& args) {
  HandleScope scope;

  GitBlob* blob = ObjectWrap::Unwrap<GitBlob>(args.This());

  int rawSize = blob->RawSize();
  std::string contents = (const char *)const_cast<void *>(blob->RawContent());

  int bufferLength = rawSize;
  Buffer* buffer = Buffer::New(const_cast<char *>(contents.c_str()), bufferLength);

  Local<Object> fastBuffer;
  MAKE_FAST_BUFFER(buffer, fastBuffer);

  return scope.Close( fastBuffer );
}

Handle<Value> GitBlob::RawSize(const Arguments& args) {
  HandleScope scope;

  GitBlob* blob = ObjectWrap::Unwrap<GitBlob>(args.This());

  return scope.Close( Integer::New(blob->RawSize()) );
}

Handle<Value> GitBlob::Close(const Arguments& args) {
  HandleScope scope;

  GitBlob* blob = ObjectWrap::Unwrap<GitBlob>(args.This());

  blob->Close();

  return scope.Close( Undefined() );
}

Handle<Value> GitBlob::CreateFromFile(const Arguments& args) {
  HandleScope scope;

  GitBlob* blob = ObjectWrap::Unwrap<GitBlob>(args.This());

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid is required and must be an Object.")));
  }

  if(args.Length() == 1 || !args[1]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repo is required and must be an Object.")));
  }

  if(args.Length() == 2 || !args[2]->IsString()) {
    return ThrowException(Exception::Error(String::New("Path is required and must be an String.")));
  }

  GitOid* oid = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject());
  GitRepo* repo = ObjectWrap::Unwrap<GitRepo>(args[1]->ToObject());

  String::Utf8Value path(args[2]);

  git_oid tmp_oid = oid->GetValue();
  int err = blob->CreateFromFile(&tmp_oid, repo->GetValue(), *path);

  return scope.Close( Integer::New(err) );
}

Handle<Value> GitBlob::CreateFromBuffer(const Arguments& args) {
  HandleScope scope;

  GitBlob* blob = ObjectWrap::Unwrap<GitBlob>(args.This());

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid is required and must be an Object.")));
  }

  if(args.Length() == 1 || !args[1]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repo is required and must be an Object.")));
  }

  if(args.Length() == 2 || !Buffer::HasInstance(args[2])) {
    return ThrowException(Exception::Error(String::New("Buffer is required and must be a Buffer.")));
  }

  GitOid* oid = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject());
  GitRepo* repo = ObjectWrap::Unwrap<GitRepo>(args[1]->ToObject());
  Local<Object> buffer = args[2]->ToObject();

  const void* data = Buffer::Data(buffer);
  size_t length = Buffer::Length(buffer);

  git_oid tmp_oid = oid->GetValue();
  int err = blob->CreateFromBuffer(&tmp_oid, repo->GetValue(), data, length);

  return scope.Close( Integer::New(err) );
}

Persistent<Function> GitBlob::constructor_template;

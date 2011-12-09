/*
 * Copyright 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * Dual licensed under the MIT and GPL licenses.
 */

#include <v8.h>
#include <node.h>
#include <node_buffer.h>
#include <string.h>

#include "../vendor/libgit2/include/git2.h"

#include "../include/utils.h"
#include "../include/repo.h"
#include "../include/blob.h"

using namespace v8;
using namespace node;

void GitBlob::Initialize (Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("Blob"));

  NODE_SET_PROTOTYPE_METHOD(constructor_template, "lookup", Lookup);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "rawContent", RawContent);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "rawSize", RawSize);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "close", Close);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "createFromFile", CreateFromFile);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "createFromBuffer", CreateFromBuffer);

  target->Set(String::NewSymbol("Blob"), constructor_template->GetFunction());
}

git_blob* GitBlob::GetValue() {
  return this->blob;
}

void GitBlob::SetValue(git_blob* blob) {
  this->blob = blob;
}

int GitBlob::Lookup(git_repository* repo, const git_oid* id) {
  return git_blob_lookup(&this->blob, repo, id);
}

const void* GitBlob::RawContent() {
  return git_blob_rawcontent(this->blob);
}

int GitBlob::RawSize() {
  return git_blob_rawsize(this->blob);
}

void GitBlob::Close() {
  git_blob_close(this->blob);
}

int CreateFromFile(git_oid* oid, git_repository* repo, const char* path) {
  return git_blob_create_fromfile(oid, repo, path);
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

  GitBlob* blob = ObjectWrap::Unwrap<GitBlob>(args.This());
  Local<Function> callback;

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repo is required and must be an Object.")));
  }

  if(args.Length() == 1 || !args[1]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid is required and must be an Object.")));
  }

  if(args.Length() == 2 || !args[2]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  callback = Local<Function>::Cast(args[2]);

  lookup_request* ar = new lookup_request();
  ar->blob = blob;
  ar->repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject());
  ar->oid = ObjectWrap::Unwrap<GitOid>(args[1]->ToObject());
  ar->callback = Persistent<Function>::New(callback);

  blob->Ref();

  eio_custom(EIO_Lookup, EIO_PRI_DEFAULT, EIO_AfterLookup, ar);
  ev_ref(EV_DEFAULT_UC);

  return scope.Close( Undefined() );
}

void GitBlob::EIO_Lookup(eio_req* req) {
  lookup_request* ar = static_cast<lookup_request* >(req->data);

  git_oid oid = ar->oid->GetValue();
  ar->err = ar->blob->Lookup(ar->repo->GetValue(), &oid);

}

int GitBlob::EIO_AfterLookup(eio_req* req) {
  lookup_request* ar = static_cast<lookup_request* >(req->data);
  ev_unref(EV_DEFAULT_UC);
  ar->blob->Unref();

  Local<Value> argv[1];
  argv[0] = Integer::New(ar->err);

  TryCatch try_catch;

  ar->callback->Call(Context::GetCurrent()->Global(), 1, argv);

  if(try_catch.HasCaught())
    FatalException(try_catch);
    
  ar->callback.Dispose();

  delete ar;

  return 0;
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

Persistent<FunctionTemplate> GitBlob::constructor_template;

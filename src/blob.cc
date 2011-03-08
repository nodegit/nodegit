/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include "../vendor/libgit2/src/git2.h"

#include "repo.h"
#include "blob.h"

using namespace v8;
using namespace node;

void Blob::Initialize (Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("Blob"));

  NODE_SET_PROTOTYPE_METHOD(constructor_template, "lookup", Lookup);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "rawContent", RawContent);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "rawSize", RawSize);

  target->Set(String::NewSymbol("Blob"), constructor_template->GetFunction());
}

git_blob* Blob::GetValue() {
  return this->blob;
}

void Blob::SetValue(git_blob* blob) {
  this->blob = blob;
}

int Blob::New(git_repository* repo) {
  return git_blob_new(&this->blob, repo);
}

int Blob::Lookup(git_repository *repo, const git_oid *id) {
  return git_blob_lookup(&this->blob, repo, id);
}

const char* Blob::RawContent() {
  return git_blob_rawcontent(this->blob);
}

int Blob::RawSize() {
  return git_blob_rawsize(this->blob);
}

Handle<Value> Blob::New(const Arguments& args) {
  HandleScope scope;

  Blob *blob = new Blob();

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repo is required and must be an Object.")));
  }

  Repo *repo = ObjectWrap::Unwrap<Repo>(args[0]->ToObject());
  int err = blob->New((git_repository *)repo);
    
  blob->Wrap(args.This());

  return args.This();
}

Handle<Value> Blob::RawContent(const Arguments& args) {
  HandleScope scope;

  Blob *blob = new Blob();

  return String::New(blob->RawContent());
}

Handle<Value> Blob::Lookup(const Arguments& args) {
  Blob *blob = ObjectWrap::Unwrap<Blob>(args.This());
  Local<Function> callback;

  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repo is required and must be a Object.")));
  }

  if(args.Length() == 1 || !args[1]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid is required and must be a Object.")));
  }

  if(args.Length() == 3 || !args[3]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  callback = Local<Function>::Cast(args[3]);

  lookup_request *ar = new lookup_request();
  ar->blob = blob;
  ar->repo = ObjectWrap::Unwrap<Repo>(args[0]->ToObject());
  ar->oid = ObjectWrap::Unwrap<Oid>(args[1]->ToObject());
  ar->callback = Persistent<Function>::New(callback);

  blob->Ref();

  eio_custom(EIO_Lookup, EIO_PRI_DEFAULT, EIO_AfterLookup, ar);
  ev_ref(EV_DEFAULT_UC);

  return Undefined();
}

int Blob::EIO_Lookup(eio_req *req) {
  lookup_request *ar = static_cast<lookup_request *>(req->data);

  int err = ar->blob->Lookup(ar->repo->GetValue(), ar->oid->GetValue());
  ar->err = Persistent<Value>::New(Integer::New(err));

  return 0;
}

int Blob::EIO_AfterLookup(eio_req *req) {
  HandleScope scope;

  lookup_request *ar = static_cast<lookup_request *>(req->data);
  ev_unref(EV_DEFAULT_UC);
  ar->blob->Unref();

  Local<Value> argv[1];
  argv[0] = Number::Cast(*ar->err);

  TryCatch try_catch;

  ar->callback->Call(Context::GetCurrent()->Global(), 1, argv);

  if(try_catch.HasCaught())
    FatalException(try_catch);
    
  ar->err.Dispose();
  ar->callback.Dispose();

  delete ar;

  return 0;
}

Handle<Value> Blob::RawSize(const Arguments& args) {
  HandleScope scope;

  Blob *blob = new Blob();

  return Integer::New(blob->RawSize());
}
Persistent<FunctionTemplate> Blob::constructor_template;

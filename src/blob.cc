/*
 * Copyright 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * Dual licensed under the MIT and GPL licenses.
 */

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include "../vendor/libgit2/include/git2.h"

#include "repo.h"
#include "../include/blob.h"

using namespace v8;
using namespace node;

namespace {
  void GitBlob::Initialize (Handle<v8::Object> target) {
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

  Handle<Value> GitBlob::New(const Arguments& args) {
    HandleScope scope;

    GitBlob* blob = new GitBlob();
    blob->Wrap(args.This());

    return args.This();
  }

  Handle<Value> GitBlob::RawContent(const Arguments& args) {
    HandleScope scope;

    GitBlob* blob = ObjectWrap::Unwrap<GitBlob>(args.This());

    return String::New((const char*)blob->RawContent());
  }

  Handle<Value> GitBlob::Lookup(const Arguments& args) {
    GitBlob* blob = ObjectWrap::Unwrap<GitBlob>(args.This());
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

    lookup_request* ar = new lookup_request();
    ar->blob = blob;
    ar->repo = ObjectWrap::Unwrap<Repo>(args[0]->ToObject());
    ar->oid = ObjectWrap::Unwrap<Oid>(args[1]->ToObject());
    ar->callback = Persistent<Function>::New(callback);

    blob->Ref();

    eio_custom(EIO_Lookup, EIO_PRI_DEFAULT, EIO_AfterLookup, ar);
    ev_ref(EV_DEFAULT_UC);

    return Undefined();
  }

  int GitBlob::EIO_Lookup(eio_req* req) {
    lookup_request* ar = static_cast<lookup_request* >(req->data);

    ar->err = ar->blob->Lookup(ar->repo->GetValue(), ar->oid->GetValue());

    return 0;
  }

  int GitBlob::EIO_AfterLookup(eio_req* req) {
    HandleScope scope;

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

  Handle<Value> GitBlob::RawSize(const Arguments& args) {
    HandleScope scope;

    GitBlob* blob = new GitBlob();

    return Integer::New(blob->RawSize());
  }

  Persistent<FunctionTemplate> GitBlob::constructor_template;
}

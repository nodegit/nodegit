/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#include <v8.h>
#include <node.h>
#include <string>

#include "../vendor/libgit2/include/git2.h"

#include "../include/repo.h"
#include "../include/reference.h"
#include "../include/oid.h"

using namespace v8;
using namespace node;

void GitReference::Initialize(Handle<Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("Ref"));

  NODE_SET_PROTOTYPE_METHOD(constructor_template, "oid", Oid);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "lookup", Lookup);

  target->Set(String::NewSymbol("Ref"), constructor_template->GetFunction());
}

git_reference* GitReference::GetValue() {
  return this->ref;
}

void GitReference::SetValue(git_reference *ref) {
  this->ref = ref;
}

int GitReference::Lookup(git_repository* repo, const char* name) {
  return git_reference_lookup(&this->ref, repo, name);
}

const git_oid* GitReference::Oid() {
  return git_reference_oid(this->ref);
}

Handle<Value> GitReference::New(const Arguments& args) {
  HandleScope scope;

  GitReference *ref = new GitReference();

  ref->Wrap(args.This());

  return args.This();
}

Handle<Value> GitReference::Lookup(const Arguments& args) {
  HandleScope scope;

  GitReference *ref = ObjectWrap::Unwrap<GitReference>(args.This());
  Local<Function> callback;

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repo is required and must be a Object.")));
  }

  if(args.Length() == 1 || !args[1]->IsString()) {
    return ThrowException(Exception::Error(String::New("Name is required and must be a String.")));
  }

  if(args.Length() == 2 || !args[2]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  callback = Local<Function>::Cast(args[2]);

  lookup_request *ar = new lookup_request();
  ar->ref = ref;
  ar->repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject());

  String::Utf8Value name(args[1]);
  ar->name = *name;

  ar->callback = Persistent<Function>::New(callback);

  ref->Ref();

  eio_custom(EIO_Lookup, EIO_PRI_DEFAULT, EIO_AfterLookup, ar);
  ev_ref(EV_DEFAULT_UC);

  return scope.Close( Undefined() );
}

void GitReference::EIO_Lookup(eio_req *req) {
  lookup_request *ar = static_cast<lookup_request *>(req->data);

  git_repository* repo = ar->repo->GetValue();

  ar->err = ar->ref->Lookup(repo, ar->name.c_str());

}

int GitReference::EIO_AfterLookup(eio_req *req) {
  HandleScope scope;

  lookup_request *ar = static_cast<lookup_request *>(req->data);
  ev_unref(EV_DEFAULT_UC);
  ar->ref->Unref();

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

Handle<Value> GitReference::Oid(const Arguments& args) {
  HandleScope scope;

  GitReference *ref = ObjectWrap::Unwrap<GitReference>(args.This());

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid is required and must be an Object.")));
  }

  GitOid *oid = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject());
  git_oid* in = const_cast<git_oid *>(ref->Oid());
  oid->SetValue(*in);

  return scope.Close( Undefined() );
}
Persistent<FunctionTemplate> GitReference::constructor_template;

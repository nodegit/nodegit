/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#include <v8.h>
#include <node.h>
#include <node_events.h>
#include <string>

#include "../vendor/libgit2/include/git2.h"

#include "repo.h"
#include "reference.h"
#include "oid.h"

using namespace v8;
using namespace node;

void Reference::Initialize(Handle<Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("Ref"));

  NODE_SET_PROTOTYPE_METHOD(constructor_template, "oid", _Oid);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "lookup", Lookup);

  target->Set(String::NewSymbol("Ref"), constructor_template->GetFunction());
}

git_reference* Reference::GetValue() {
  return this->ref;
}

void Reference::SetValue(git_reference *ref) {
  this->ref = ref;
}

int Reference::Lookup(git_repository* repo, const char* name) {
  return git_reference_lookup(&this->ref, repo, name);
}

const git_oid* Reference::_Oid() {
  return git_reference_oid(*&this->ref);
}

Handle<Value> Reference::New(const Arguments& args) {
  HandleScope scope;

  Reference *ref = new Reference();

  ref->Wrap(args.This());

  return args.This();
}

Handle<Value> Reference::Lookup(const Arguments& args) {
  Reference *ref = ObjectWrap::Unwrap<Reference>(args.This());
  Local<Function> callback;

  HandleScope scope;

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
  ar->repo = ObjectWrap::Unwrap<Repo>(args[0]->ToObject());

  String::Utf8Value name(args[1]);
  ar->name = *name;

  ar->callback = Persistent<Function>::New(callback);

  ref->Ref();

  eio_custom(EIO_Lookup, EIO_PRI_DEFAULT, EIO_AfterLookup, ar);
  ev_ref(EV_DEFAULT_UC);

  return Undefined();
}

int Reference::EIO_Lookup(eio_req *req) {
  lookup_request *ar = static_cast<lookup_request *>(req->data);

  git_repository* repo = ar->repo->GetValue();

  ar->err = ar->ref->Lookup(repo, ar->name.c_str());

  return 0;
}

int Reference::EIO_AfterLookup(eio_req *req) {
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

Handle<Value> Reference::_Oid(const Arguments& args) {
  Reference *ref = ObjectWrap::Unwrap<Reference>(args.This());
  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid is required and must be an Object.")));
  }

  Oid *oid = ObjectWrap::Unwrap<Oid>(args[0]->ToObject());
  oid->SetValue( const_cast<git_oid *>(ref->_Oid()) );

  return Undefined();
}
Persistent<FunctionTemplate> Reference::constructor_template;

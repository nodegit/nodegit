/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include <git2.h>

#include "commit.h"
#include "repo.h"
#include "oid.h"

using namespace v8;
using namespace node;

void Commit::Initialize(Handle<Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("Commit"));

  NODE_SET_PROTOTYPE_METHOD(constructor_template, "lookup", Lookup);

  target->Set(String::NewSymbol("Commit"), constructor_template->GetFunction());
}

int Commit::Lookup(Repo *repo, Oid *oid) {
  return git_commit_lookup(&this->commit, repo->GetValue(), oid->GetValue());
}

Handle<Value> Commit::New(const Arguments& args) {
  HandleScope scope;

  Commit *commit = new Commit();
  commit->Wrap(args.This());

  return args.This();
}

Handle<Value> Commit::Lookup(const Arguments& args) {
  Commit *commit = ObjectWrap::Unwrap<Commit>(args.This());
  Local<Function> callback;

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

  lookup_request *ar = new lookup_request();
  callback = Local<Function>::Cast(args[2]);
  ar->commit = commit;
  ar->repo = ObjectWrap::Unwrap<Repo>(args[0]->ToObject());
  ar->oid = ObjectWrap::Unwrap<Oid>(args[1]->ToObject());
  ar->callback = Persistent<Function>::New(callback);

  commit->Ref();

  eio_custom(EIO_Lookup, EIO_PRI_DEFAULT, EIO_AfterLookup, ar);
  ev_ref(EV_DEFAULT_UC);

  return Undefined();
}

int Commit::EIO_Lookup(eio_req *req) {
  lookup_request *ar = static_cast<lookup_request *>(req->data);

  ar->err = Persistent<Value>::New(Integer::New(ar->commit->Lookup(ar->repo, ar->oid)));

  return 0;
}

int Commit::EIO_AfterLookup(eio_req *req) {
  HandleScope scope;

  lookup_request *ar = static_cast<lookup_request *>(req->data);
  ev_unref(EV_DEFAULT_UC);
  ar->commit->Unref();

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
Persistent<FunctionTemplate> Commit::constructor_template;

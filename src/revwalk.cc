/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include "../vendor/libgit2/src/git2.h"

#include "revwalk.h"
#include "repo.h"
#include "commit.h"

using namespace v8;
using namespace node;

void RevWalk::Initialize(Handle<Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("RevWalk"));

  NODE_SET_PROTOTYPE_METHOD(constructor_template, "reset", Reset);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "push", Push);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "next", Next);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "free", Free);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "repository", Repository);

  Local<Object> sort = Object::New();

  sort->Set(String::New("NONE"), Integer::New(0));
  sort->Set(String::New("TOPOLOGICAL"), Integer::New(1));
  sort->Set(String::New("TIME"), Integer::New(2));
  sort->Set(String::New("REVERSE"), Integer::New(4));

  constructor_template->Set(String::New("sort"), sort);

  target->Set(String::NewSymbol("RevWalk"), constructor_template->GetFunction());
}

git_revwalk* RevWalk::GetValue() {
  return this->revwalk;
}

void RevWalk::SetValue(git_revwalk* revwalk) {
  this->revwalk = revwalk;
}

int RevWalk::New(Repo *repo) {
  return git_revwalk_new(&this->revwalk, repo->GetValue());
}

void RevWalk::Reset() {
  git_revwalk_reset(this->revwalk);
}

int RevWalk::Push(Commit *commit) {
  return git_revwalk_push(this->revwalk, commit->GetValue());
}

// Not for 0.0.1
//int RevWalk::Hide() {
//  git_revwalk_hide(this->revwalk);
//}

int RevWalk::Next(git_commit **commit) {
  return git_revwalk_next(commit, this->revwalk);
}

void RevWalk::Free() {
  git_revwalk_free(this->revwalk);
}

git_repository* RevWalk::Repository() {
	return git_revwalk_repository(this->revwalk);
}

Handle<Value> RevWalk::New(const Arguments& args) {
  HandleScope scope;

  RevWalk *revwalk = new RevWalk();

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repo is required and must be an Object.")));
  }

  Repo *repo = ObjectWrap::Unwrap<Repo>(args[0]->ToObject());
  revwalk->New(repo);

  revwalk->Wrap(args.This());

  return args.This();
}

Handle<Value> RevWalk::Reset(const Arguments& args) {
  HandleScope scope;

  RevWalk *revwalk = ObjectWrap::Unwrap<RevWalk>(args.This());

  revwalk->Reset();

  return Undefined();
}

Handle<Value> RevWalk::Push(const Arguments& args) {
  HandleScope scope;

  RevWalk *revwalk = ObjectWrap::Unwrap<RevWalk>(args.This());
  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Commit is required and must be an Object.")));
  }

  Commit *commit = ObjectWrap::Unwrap<Commit>(args[0]->ToObject());
  int err = revwalk->Push(commit);

  return Local<Value>::New(Integer::New(err));
}

Handle<Value> RevWalk::Next(const Arguments& args) {
  RevWalk *revwalk = ObjectWrap::Unwrap<RevWalk>(args.This());
  Local<Function> callback;

  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Commit is required and must be an Object.")));
  }

  if(args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  callback = Local<Function>::Cast(args[1]);

  next_request *ar = new next_request();
  ar->revwalk = revwalk;
  ar->commit = ObjectWrap::Unwrap<Commit>(args[0]->ToObject());
  ar->callback = Persistent<Function>::New(callback);

  revwalk->Ref();

  eio_custom(EIO_Next, EIO_PRI_DEFAULT, EIO_AfterNext, ar);
  ev_ref(EV_DEFAULT_UC);

  return Undefined();
}

int RevWalk::EIO_Next(eio_req *req) {
  next_request *ar = static_cast<next_request *>(req->data);
  git_commit* ref = ar->commit->GetValue();

  ar->err = Persistent<Value>::New(Integer::New(ar->revwalk->Next(&ref)));

  ar->commit->SetValue(ref);

  return 0;
}

int RevWalk::EIO_AfterNext(eio_req *req) {
  HandleScope scope;

  next_request *ar = static_cast<next_request *>(req->data);
  ev_unref(EV_DEFAULT_UC);
  ar->revwalk->Unref();

  Local<Value> argv[1];
  argv[0] = *ar->err;

  TryCatch try_catch;

  ar->callback->Call(Context::GetCurrent()->Global(), 1, argv);

  if(try_catch.HasCaught())
    FatalException(try_catch);
    
  ar->err.Dispose();
  ar->callback.Dispose();

  delete ar;

  return 0;
}

Handle<Value> RevWalk::Free(const Arguments& args) {
  RevWalk *revwalk = ObjectWrap::Unwrap<RevWalk>(args.This());

  HandleScope scope;

  revwalk->Free();

  return Undefined();
}

Handle<Value> RevWalk::Repository(const Arguments& args) {
  HandleScope scope;

  RevWalk *revwalk = new RevWalk();

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repo is required and must be an Object.")));
  }

  Repo *repo = ObjectWrap::Unwrap<Repo>(args[0]->ToObject());
  repo->SetValue(revwalk->Repository());

  return Undefined();
}
Persistent<FunctionTemplate> RevWalk::constructor_template;

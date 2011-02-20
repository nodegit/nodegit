/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include <git2.h>

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

  NODE_SET_PROTOTYPE_METHOD(constructor_template, "alloc", Alloc);

  target->Set(String::NewSymbol("RevWalk"), constructor_template->GetFunction());
}

git_revwalk* RevWalk::GetValue() {
  return this->revwalk;
}

int RevWalk::Alloc(Repo *repo) {
  return git_revwalk_new(&this->revwalk, repo->GetValue());
}

Handle<Value> RevWalk::New(const Arguments& args) {
  HandleScope scope;

  RevWalk *revwalk = new RevWalk();
  revwalk->Wrap(args.This());

  return args.This();
}

Handle<Value> RevWalk::Alloc(const Arguments& args) {
  RevWalk *revwalk = ObjectWrap::Unwrap<RevWalk>(args.This());
  Local<Function> callback;

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repo is required and must be an Object.")));
  }

  if(args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  callback = Local<Function>::Cast(args[1]);

  int err = revwalk->Alloc(ObjectWrap::Unwrap<Repo>(args[0]->ToObject()));

  Local<Value> argv[1];
  argv[0] = Local<Value>::New(Integer::New(err));

  TryCatch try_catch;

  callback->Call(Context::GetCurrent()->Global(), 1, argv);

  if(try_catch.HasCaught())
    FatalException(try_catch);

  return Undefined();
}

Persistent<FunctionTemplate> RevWalk::constructor_template;

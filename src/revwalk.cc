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

  NODE_SET_PROTOTYPE_METHOD(constructor_template, "push", Push);

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

int RevWalk::Push(Commit *commit) {
  return git_revwalk_push(*&this->revwalk, commit->GetValue());
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

Handle<Value> RevWalk::Push(const Arguments& args) {
  HandleScope scope;

  RevWalk *revwalk = new RevWalk();

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Commit is required and must be an Object.")));
  }

  Commit *commit = ObjectWrap::Unwrap<Commit>(args[0]->ToObject());
  int err = revwalk->Push(commit);

  return Local<Value>::New(Integer::New(err));
}
Persistent<FunctionTemplate> RevWalk::constructor_template;

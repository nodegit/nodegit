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

  HandleScope scope;

  //if(args.Length() == 0 || !args[0]->IsString()) {
   // return ThrowException(Exception::Error(String::New("Object id is required and must be a hex formatted String.")));
  //}

  Repo *repo = ObjectWrap::Unwrap<Repo>(args[0]->ToObject());
  Oid *oid = ObjectWrap::Unwrap<Oid>(args[1]->ToObject());

  return Local<Value>::New( Integer::New(commit->Lookup(repo, oid)) );
}

Persistent<FunctionTemplate> Commit::constructor_template;

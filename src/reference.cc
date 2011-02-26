/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include <git2.h>

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

  NODE_SET_PROTOTYPE_METHOD(constructor_template, "oid", Oid);

  target->Set(String::NewSymbol("Ref"), constructor_template->GetFunction());
}

git_reference* Reference::GetValue() {
  return this->ref;
}

void Reference::SetValue(git_reference *ref) {
  this->ref = ref;
}

int Reference::New(git_repository* repo) {
  return git_reference_new(&this->ref, repo);
}

const git_oid* Reference::Oid() {
  return git_reference_oid(this->ref);
}

Handle<Value> Reference::New(const Arguments& args) {
  HandleScope scope;

  Reference *ref = new Reference();

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repo is required and must be an Object.")));
  }

  Repo *repo = ObjectWrap::Unwrap<Repo>(args[0]->ToObject());
  ref->New((git_repository *)repo);

  ref->Wrap(args.This());

  return args.This();
}

Handle<Value> Reference::Oid(const Arguments& args) {
  Reference *ref = ObjectWrap::Unwrap<Reference>(args.This());
  Local<Function> callback;

  HandleScope scope;

//  if(args.Length() == 0 || !args[0]->IsObject()) {
//    return ThrowException(Exception::Error(String::New("Oid is required and must be an Object.")));
//  }
//
//  callback = Local<Function>::Cast(args[2]);

  //Oid *oid = ObjectWrap::Unwrap<Oid>(args[0]->ToObject());
  //oid->SetValue((git_oid *)ref->Oid());
  //
  const git_oid* oid = ref->Oid();
  Local<Object> obj;

  //Oid *t = new Oid();
  //t->SetValue((git_oid *)oid);
  //return t->WrapObj(obj);
  return callback;
}

Persistent<FunctionTemplate> Reference::constructor_template;

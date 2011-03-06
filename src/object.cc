/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include <git2.h>

#include "object.h"
#include "repo.h"
#include "oid.h"

using namespace v8;
using namespace node;

void GitObject::Initialize (Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("Object"));

  NODE_SET_PROTOTYPE_METHOD(constructor_template, "id", Id);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "owner", Owner);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "free", Free);

  target->Set(String::NewSymbol("Object"), constructor_template->GetFunction());
}

git_object* GitObject::GetValue() {
  return this->obj;
}

void GitObject::SetValue(git_object* obj) {
  this->obj = obj;
}

const git_oid* GitObject::Id() {
  return git_object_id(this->obj);
}

git_repository* GitObject::Owner() {
  return git_object_owner(this->obj);
}

void GitObject::Free() {
  git_object_free(this->obj);
}

Handle<Value> GitObject::New(const Arguments& args) {
  HandleScope scope;

  GitObject *obj = new GitObject();

  obj->Wrap(args.This());

  return args.This();
}

Handle<Value> GitObject::Id(const Arguments& args) {
  HandleScope scope;

  GitObject *obj = ObjectWrap::Unwrap<GitObject>(args.This());
  
  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid is required and must be an Object.")));
  }

  Oid *oid = ObjectWrap::Unwrap<Oid>(args[0]->ToObject());

  oid->SetValue(const_cast<git_oid *>(obj->Id()));

  return Undefined();
}

Handle<Value> GitObject::Owner(const Arguments& args) {
  HandleScope scope;

  GitObject *obj = ObjectWrap::Unwrap<GitObject>(args.This());
  
  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repo is required and must be an Object.")));
  }

  Repo *repo = ObjectWrap::Unwrap<Repo>(args[0]->ToObject());

  repo->SetValue(obj->Owner());

  return Undefined();
}

Handle<Value> GitObject::Free(const Arguments& args) {
  HandleScope scope;

  GitObject *obj = ObjectWrap::Unwrap<GitObject>(args.This());

  obj->Free();

  return Undefined();
}
Persistent<FunctionTemplate> GitObject::constructor_template;

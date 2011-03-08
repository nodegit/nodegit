/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include "../vendor/libgit2/src/git2.h"

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
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "type", Type);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "owner", Owner);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "free", Free);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "type2String", Type2String);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "string2Type", String2Type);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "typeIsLoose", TypeIsLoose);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "size", Size);

  Local<Object> type = Object::New();

  type->Set(String::New("ANY"), Integer::New(-2));
  type->Set(String::New("BAD"), Integer::New(-1));
  type->Set(String::New("_EXT1"), Integer::New(0));
  type->Set(String::New("COMMIT"), Integer::New(1));
  type->Set(String::New("TREE"), Integer::New(2));
  type->Set(String::New("BLOB"), Integer::New(3));
  type->Set(String::New("TAG"), Integer::New(4));
  type->Set(String::New("_EXT2"), Integer::New(5));
  type->Set(String::New("OFS_DELTA"), Integer::New(6));
  type->Set(String::New("REF_DELTA"), Integer::New(7));

  constructor_template->Set(String::New("type"), type);

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

git_otype GitObject::Type() {
  return git_object_type(this->obj);
}

git_repository* GitObject::Owner() {
  return git_object_owner(this->obj);
}

void GitObject::Free() {
  git_object_free(this->obj);
}

const char* GitObject::Type2String(git_otype type) {
  return git_object_type2string(type);
}

git_otype GitObject::String2Type(const char* type) {
  return git_object_string2type(type);
}

int GitObject::TypeIsLoose(git_otype type) {
  return git_object_typeisloose(type);
}

size_t GitObject::Size(git_otype type) {
  return git_object__size(type);
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

Handle<Value> GitObject::Type(const Arguments& args) {
  HandleScope scope;

  GitObject *obj = ObjectWrap::Unwrap<GitObject>(args.This());
  
  return Integer::New(obj->Type());
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

Handle<Value> GitObject::Type2String(const Arguments& args) {
  HandleScope scope;

  GitObject *obj = ObjectWrap::Unwrap<GitObject>(args.This());
  
  if(args.Length() == 0 || !args[0]->IsNumber()) {
    return ThrowException(Exception::Error(String::New("Type is required and must be a Number.")));
  }

  git_otype type = (git_otype)args[0]->ToInteger()->Value();

  return String::New(obj->Type2String(type));
}

Handle<Value> GitObject::String2Type(const Arguments& args) {
  HandleScope scope;

  GitObject *obj = ObjectWrap::Unwrap<GitObject>(args.This());
  
  if(args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("Type is required and must be a String.")));
  }

  String::Utf8Value type(args[0]);

  return Integer::New(obj->String2Type(*type));
}

Handle<Value> GitObject::TypeIsLoose(const Arguments& args) {
  HandleScope scope;

  GitObject *obj = ObjectWrap::Unwrap<GitObject>(args.This());
  
  if(args.Length() == 0 || !args[0]->IsNumber()) {
    return ThrowException(Exception::Error(String::New("Type is required and must be a Number.")));
  }

  git_otype type = (git_otype)args[0]->ToInteger()->Value();

  return Integer::New(obj->TypeIsLoose(type));
}

Handle<Value> GitObject::Size(const Arguments& args) {
  HandleScope scope;

  GitObject *obj = ObjectWrap::Unwrap<GitObject>(args.This());
  
  if(args.Length() == 0 || !args[0]->IsNumber()) {
    return ThrowException(Exception::Error(String::New("Type is required and must be a Number.")));
  }

  git_otype type = (git_otype)args[0]->ToInteger()->Value();

  return Integer::New(obj->Size(type));
}

Persistent<FunctionTemplate> GitObject::constructor_template;

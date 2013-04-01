/*
 * Copyright 2013, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * @author Michael Robinson @codeofinterest <mike@pagesofinterest.net>
 *
 * Dual licensed under the MIT and GPL licenses.
 */

#include <v8.h>
#include <node.h>

#include "cvv8/v8-convert.hpp"
#include "git2.h"

#include "../include/object.h"
#include "../include/repo.h"
#include "../include/oid.h"

using namespace v8;
using namespace node;

namespace cvv8 {
  template <>
  struct NativeToJS<git_otype> : NativeToJS<int32_t> {};
}

void GitObject::Initialize (Handle<v8::Object> target) {
  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Object"));

  NODE_SET_PROTOTYPE_METHOD(tpl, "id", Id);
  NODE_SET_PROTOTYPE_METHOD(tpl, "type", Type);
  NODE_SET_PROTOTYPE_METHOD(tpl, "owner", Owner);
  NODE_SET_PROTOTYPE_METHOD(tpl, "type2String", Type2String);
  NODE_SET_PROTOTYPE_METHOD(tpl, "string2Type", String2Type);
  NODE_SET_PROTOTYPE_METHOD(tpl, "typeIsLoose", TypeIsLoose);
  NODE_SET_PROTOTYPE_METHOD(tpl, "size", Size);

  Local<Object> types = Object::New();

  types->Set(String::New("GIT_OBJ_ANY"), cvv8::CastToJS(GIT_OBJ_ANY));
  types->Set(String::New("GIT_OBJ_BAD"), cvv8::CastToJS(GIT_OBJ_BAD));
  types->Set(String::New("GIT_OBJ__EXT1"), cvv8::CastToJS(GIT_OBJ__EXT1));
  types->Set(String::New("GIT_OBJ_COMMIT"), cvv8::CastToJS(GIT_OBJ_COMMIT));
  types->Set(String::New("GIT_OBJ_TREE"), cvv8::CastToJS(GIT_OBJ_TREE));
  types->Set(String::New("GIT_OBJ_BLOB"), cvv8::CastToJS(GIT_OBJ_BLOB));
  types->Set(String::New("GIT_OBJ_TAG"), cvv8::CastToJS(GIT_OBJ_TAG));
  types->Set(String::New("GIT_OBJ__EXT2"), cvv8::CastToJS(GIT_OBJ__EXT2));
  types->Set(String::New("GIT_OBJ_OFS_DELTA"), cvv8::CastToJS(GIT_OBJ_OFS_DELTA));
  types->Set(String::New("GIT_OBJ_REF_DELTA"), cvv8::CastToJS(GIT_OBJ_REF_DELTA));

  tpl->Set(String::New("types"), types);

  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Object"), constructor_template);
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

  return scope.Close( args.This() );
}

Handle<Value> GitObject::Id(const Arguments& args) {
  HandleScope scope;

  GitObject *obj = ObjectWrap::Unwrap<GitObject>(args.This());

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid is required and must be an Object.")));
  }

  GitOid *oid = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject());

  oid->SetValue(*const_cast<git_oid *>(obj->Id()));

  return scope.Close( Undefined() );
}

Handle<Value> GitObject::Type(const Arguments& args) {
  HandleScope scope;

  GitObject *obj = ObjectWrap::Unwrap<GitObject>(args.This());

  return scope.Close( Integer::New(obj->Type()) );
}

Handle<Value> GitObject::Owner(const Arguments& args) {
  HandleScope scope;

  GitObject *obj = ObjectWrap::Unwrap<GitObject>(args.This());

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repo is required and must be an Object.")));
  }

  GitRepo *repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject());

  repo->SetValue(obj->Owner());

  return scope.Close( Undefined() );
}

Handle<Value> GitObject::Type2String(const Arguments& args) {
  HandleScope scope;

  GitObject *obj = ObjectWrap::Unwrap<GitObject>(args.This());

  if(args.Length() == 0 || !args[0]->IsNumber()) {
    return ThrowException(Exception::Error(String::New("Type is required and must be a Number.")));
  }

  git_otype type = (git_otype)args[0]->ToInteger()->Value();

  return scope.Close( String::New(obj->Type2String(type)) );
}

Handle<Value> GitObject::String2Type(const Arguments& args) {
  HandleScope scope;

  GitObject *obj = ObjectWrap::Unwrap<GitObject>(args.This());

  if(args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("Type is required and must be a String.")));
  }

  String::Utf8Value type(args[0]);

  return scope.Close( Integer::New(obj->String2Type(*type)) );
}

Handle<Value> GitObject::TypeIsLoose(const Arguments& args) {
  HandleScope scope;

  GitObject *obj = ObjectWrap::Unwrap<GitObject>(args.This());

  if(args.Length() == 0 || !args[0]->IsNumber()) {
    return ThrowException(Exception::Error(String::New("Type is required and must be a Number.")));
  }

  git_otype type = (git_otype)args[0]->ToInteger()->Value();

  return scope.Close( Integer::New(obj->TypeIsLoose(type)) );
}

Handle<Value> GitObject::Size(const Arguments& args) {
  HandleScope scope;

  GitObject *obj = ObjectWrap::Unwrap<GitObject>(args.This());

  if(args.Length() == 0 || !args[0]->IsNumber()) {
    return ThrowException(Exception::Error(String::New("Type is required and must be a Number.")));
  }

  git_otype type = (git_otype)args[0]->ToInteger()->Value();

  return scope.Close( Integer::New(obj->Size(type)) );
}

Persistent<Function> GitObject::constructor_template;

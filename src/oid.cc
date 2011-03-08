/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include "../vendor/libgit2/src/git2.h"

#include "oid.h"

using namespace v8;
using namespace node;

void Oid::Initialize(Handle<Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("Oid"));

  NODE_SET_PROTOTYPE_METHOD(constructor_template, "mkstr", Mkstr);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "mkraw", Mkraw);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "fmt", Fmt);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "toString", ToString);

  target->Set(String::NewSymbol("Oid"), constructor_template->GetFunction());
}

git_oid* Oid::GetValue() {
  return &this->oid;
}

void Oid::SetValue(git_oid* oid) {
  this->oid = *oid;
}

int Oid::Mkstr(const char* id) {
  return git_oid_mkstr(&this->oid, id);
}

void Oid::Mkraw(const unsigned char* raw) {
  git_oid_mkraw(&this->oid, raw);
}

char* Oid::Fmt(char* buffer) {
  git_oid_fmt(*&buffer, &this->oid);
}

void Oid::PathFmt(char* str) {
  git_oid_pathfmt(str, &this->oid);
}

char* Oid::AllocFmt() {
  return git_oid_allocfmt(&this->oid);
}

char* Oid::ToString(char* buffer, size_t bufferSize) {
  git_oid_to_string(*&buffer, bufferSize, &this->oid);
}

void Oid::Cpy(git_oid* out) {
  git_oid_cpy(out, &this->oid);
}

int Oid::Cmp(const git_oid* a, const git_oid* b) {
  return git_oid_cmp(a, b);
}

Handle<Value> Oid::New(const Arguments& args) {
  HandleScope scope;

  Oid *oid = new Oid();
  oid->Wrap(args.This());

  return args.This();
}

Handle<Value> Oid::Mkstr(const Arguments& args) {
  Oid *oid = ObjectWrap::Unwrap<Oid>(args.This());

  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("Object id is required and must be a hex formatted String.")));
  }

  String::Utf8Value id(Local<Value>::New(args[0]));

  return Local<Value>::New( Integer::New(oid->Mkstr(*id)) );
}

Handle<Value> Oid::Mkraw(const Arguments& args) {
  Oid *oid = ObjectWrap::Unwrap<Oid>(args.This());

  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("Raw object id is required.")));
  }

  String::Utf8Value raw(Local<Value>::New(args[0]));
  oid->Mkraw((const unsigned char*)*raw);

  return Local<Value>::New(args.This());
}

Handle<Value> Oid::Fmt(const Arguments& args) {
  Oid *oid = ObjectWrap::Unwrap<Oid>(args.This());

  HandleScope scope;

  char buffer[40];
  oid->Fmt(buffer);
  return String::New(buffer);
}

Handle<Value> Oid::PathFmt(const Arguments& args) {
  Oid *oid = ObjectWrap::Unwrap<Oid>(args.This());

  HandleScope scope;

  char buffer[41];
  oid->PathFmt(buffer);
  return String::New(buffer);
}

Handle<Value> Oid::AllocFmt(const Arguments& args) {
  Oid *oid = ObjectWrap::Unwrap<Oid>(args.This());

  HandleScope scope;

  return String::New(oid->AllocFmt());
}

Handle<Value> Oid::ToString(const Arguments& args) {
  Oid *oid = ObjectWrap::Unwrap<Oid>(args.This());

  HandleScope scope;
  
  if(args.Length() == 0 || !args[0]->IsNumber()) {
    return ThrowException(Exception::Error(String::New("Length argument is required and must be a Number.")));
  }

  char buffer[Int32::Cast(*args[0])->Value()+1];
  oid->ToString(buffer, sizeof(buffer));
  return String::New(buffer);
}

Handle<Value> Oid::Cpy(const Arguments& args) {
  Oid *oid = ObjectWrap::Unwrap<Oid>(args.This());

  HandleScope scope;
  
  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid argument is required and must be a Object.")));
  }

  Oid *clone = ObjectWrap::Unwrap<Oid>(args[0]->ToObject());
  
  git_oid *out;
  oid->Cpy(out);
  clone->SetValue(out);

  return Undefined();
}

Handle<Value> Oid::Cmp(const Arguments& args) {
  Oid *oid = ObjectWrap::Unwrap<Oid>(args.This());

  HandleScope scope;
  
  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid argument is required and must be a Object.")));
  }
  
  if(args.Length() == 1 || !args[1]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid argument is required and must be a Object.")));
  }

  Oid *a = ObjectWrap::Unwrap<Oid>(args[0]->ToObject());
  Oid *b = ObjectWrap::Unwrap<Oid>(args[1]->ToObject());

  int cmp = oid->Cmp(a->GetValue(), b->GetValue());

  return Integer::New(cmp);
}
Persistent<FunctionTemplate> Oid::constructor_template;

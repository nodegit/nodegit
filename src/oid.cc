/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#include <v8.h>
#include <node.h>
#include <node_events.h>

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

Handle<Value> Oid::WrapObj(Local<Object> obj) {
  this->Wrap(obj);
  return obj;
}

git_oid* Oid::GetValue() {
  return &this->oid;
}

void Oid::SetValue(git_oid *oid) {
  this->oid = *oid;
}

int Oid::Mkstr(const char* id) {
  return git_oid_mkstr(&this->oid, id);
}

void Oid::Mkraw(const unsigned char *raw) {
  git_oid_mkraw(&this->oid, raw);
}

char* Oid::Fmt() {
  char buffer[40];
  git_oid_fmt(buffer, &this->oid);

  return buffer;
}

char* Oid::ToString(int len) {
  char buffer[len];
  git_oid_to_string(*&buffer, sizeof(buffer), (const git_oid*)&this->oid);

  return buffer;
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

  return String::New(oid->Fmt());
}

Handle<Value> Oid::ToString(const Arguments& args) {
  Oid *oid = ObjectWrap::Unwrap<Oid>(args.This());

  HandleScope scope;
  
  if(args.Length() == 0 || !args[0]->IsNumber()) {
    return ThrowException(Exception::Error(String::New("Length argument is required and must be a Number.")));
  }

  int len = Local<Integer>::Cast(args[0])->Value();

  return String::New(oid->ToString(len));
}

Persistent<FunctionTemplate> Oid::constructor_template;

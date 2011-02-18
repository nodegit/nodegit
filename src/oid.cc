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

  target->Set(String::NewSymbol("Oid"), constructor_template->GetFunction());
}

int Oid::Mkstr(const char* id) {
  return git_oid_mkstr(&this->oid, id);
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

Persistent<FunctionTemplate> Oid::constructor_template;

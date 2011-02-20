/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include <git2.h>

#include "ref.h"

using namespace v8;
using namespace node;

void Ref::Initialize(Handle<Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("Ref"));

  target->Set(String::NewSymbol("Ref"), constructor_template->GetFunction());
}

git_reference* Ref::GetValue() {
  return this->ref;
}

void Ref::SetValue(git_reference *ref) {
  this->ref = ref;
}

Handle<Value> Ref::New(const Arguments& args) {
  HandleScope scope;

  Ref *ref = new Ref();
  ref->Wrap(args.This());

  return args.This();
}

Persistent<FunctionTemplate> Ref::constructor_template;

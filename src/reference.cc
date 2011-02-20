/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include <git2.h>

#include "reference.h"

using namespace v8;
using namespace node;

void Reference::Initialize(Handle<Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("Reference"));

  target->Set(String::NewSymbol("Reference"), constructor_template->GetFunction());
}

git_reference* Reference::GetValue() {
  return this->ref;
}

void Reference::SetValue(git_reference *ref) {
  this->ref = ref;
}

Handle<Value> Reference::New(const Arguments& args) {
  HandleScope scope;

  Reference *ref = new Reference();
  ref->Wrap(args.This());

  return args.This();
}

Persistent<FunctionTemplate> Reference::constructor_template;

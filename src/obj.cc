/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include <git2.h>

#include "obj.h"
#include "repo.h"

using namespace v8;
using namespace node;

void Obj::Initialize (Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("Obj"));

  target->Set(String::NewSymbol("Obj"), constructor_template->GetFunction());
}

git_object* Obj::GetValue() {
  return this->obj;
}

void Obj::SetValue(git_object* obj) {
  this->obj = obj;
}

Handle<Value> Obj::New(const Arguments& args) {
  HandleScope scope;

  Obj *obj = new Obj();

  obj->Wrap(args.This());

  return args.This();
}
Persistent<FunctionTemplate> Obj::constructor_template;

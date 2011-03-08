/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include "../vendor/libgit2/src/git2.h"

#include "error.h"

using namespace v8;
using namespace node;

void Error::Initialize (Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("Error"));

  NODE_SET_PROTOTYPE_METHOD(constructor_template, "strError", StrError);

  target->Set(String::NewSymbol("Error"), constructor_template->GetFunction());
}

Handle<Value> Error::New(const Arguments& args) {
  HandleScope scope;

  Error *error = new Error();
  error->Wrap(args.This());

  return args.This();
}

Handle<Value> Error::StrError(const Arguments& args) {
  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsNumber()) {
    return ThrowException(Exception::Error(String::New("Error is required and must be a Number.")));
  }
  Local<Integer> err = Local<Integer>::Cast(args[0]);

  return String::New(git_strerror(err->Value()));
}
Persistent<FunctionTemplate> Error::constructor_template;

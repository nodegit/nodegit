/*
 * Copyright 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * Dual licensed under the MIT and GPL licenses.
 */

#include <v8.h>
#include <node.h>

#include "../vendor/libgit2/include/git2.h"

#include "../include/error.h"

using namespace v8;
using namespace node;

void GitError::Initialize (Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("Error"));

  NODE_SET_PROTOTYPE_METHOD(constructor_template, "strError", StrError);

  target->Set(String::NewSymbol("Error"), constructor_template->GetFunction());
}

const char* GitError::StrError(int err) {
  return git_strerror(err);
}

Handle<Value> GitError::New(const Arguments& args) {
  HandleScope scope;

  GitError *error = new GitError();
  error->Wrap(args.This());

  return scope.Close( args.This() );
}

Handle<Value> GitError::StrError(const Arguments& args) {
  HandleScope scope;

  GitError* error = ObjectWrap::Unwrap<GitError>(args.This());

  if(args.Length() == 0 || !args[0]->IsNumber()) {
    return ThrowException(Exception::Error(String::New("Error is required and must be a Number.")));
  }

  Local<Integer> err = Local<Integer>::Cast(args[0]);

  return scope.Close( String::New(error->StrError(err->Value())) );
}

Persistent<FunctionTemplate> GitError::constructor_template;

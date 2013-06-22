/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/time.h"

#include "../include/functions/utilities.h"
#include "../include/functions/string.h"

using namespace v8;
using namespace node;

GitTime::GitTime(git_time *raw) {
  this->raw = raw;
}

GitTime::~GitTime() {
  free(this->raw);
}

void GitTime::Initialize(Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Time"));


  NODE_SET_PROTOTYPE_METHOD(tpl, "time", Time);
  NODE_SET_PROTOTYPE_METHOD(tpl, "offset", Offset);

  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Time"), constructor_template);
}

Handle<Value> GitTime::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return ThrowException(Exception::Error(String::New("git_time is required.")));
  }

  GitTime* object = new GitTime((git_time *) External::Unwrap(args[0]));
  object->Wrap(args.This());

  return scope.Close(args.This());
}

git_time *GitTime::GetValue() {
  return this->raw;
}


Handle<Value> GitTime::Time(const Arguments& args) {
  HandleScope scope;
  git_time_t field = ObjectWrap::Unwrap<GitTime>(args.This())->GetValue()->time;

  return scope.Close(Integer::New(field));
}
Handle<Value> GitTime::Offset(const Arguments& args) {
  HandleScope scope;
  int field = ObjectWrap::Unwrap<GitTime>(args.This())->GetValue()->offset;

  return scope.Close(Int32::New(field));
}

Persistent<Function> GitTime::constructor_template;

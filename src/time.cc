/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/functions/copy.h"

#include "../include/time.h"

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

Handle<Value> GitTime::New(void *raw) {
  HandleScope scope;
  Handle<Value> argv[1] = { External::New((void *)raw) };
  return scope.Close(GitTime::constructor_template->NewInstance(1, argv));
}

git_time *GitTime::GetValue() {
  return this->raw;
}


Handle<Value> GitTime::Time(const Arguments& args) {
  HandleScope scope;
    Handle<Value> to;

  git_time_t time =
    ObjectWrap::Unwrap<GitTime>(args.This())->GetValue()->time;

    to = Integer::New(time);
  return scope.Close(to);
}

Handle<Value> GitTime::Offset(const Arguments& args) {
  HandleScope scope;
    Handle<Value> to;

  int offset =
    ObjectWrap::Unwrap<GitTime>(args.This())->GetValue()->offset;

    to = Int32::New(offset);
  return scope.Close(to);
}

Persistent<Function> GitTime::constructor_template;

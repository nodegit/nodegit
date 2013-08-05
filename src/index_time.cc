/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/functions/copy.h"

#include "../include/index_time.h"

using namespace v8;
using namespace node;

GitIndexTime::GitIndexTime(git_index_time *raw) {
  this->raw = raw;
}

GitIndexTime::~GitIndexTime() {
  free(this->raw);
}

void GitIndexTime::Initialize(Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("IndexTime"));


  NODE_SET_PROTOTYPE_METHOD(tpl, "seconds", Seconds);
  NODE_SET_PROTOTYPE_METHOD(tpl, "nanoseconds", Nanoseconds);

  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("IndexTime"), constructor_template);
}

Handle<Value> GitIndexTime::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return ThrowException(Exception::Error(String::New("git_index_time is required.")));
  }

  GitIndexTime* object = new GitIndexTime((git_index_time *) External::Unwrap(args[0]));
  object->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> GitIndexTime::New(void *raw) {
  HandleScope scope;
  Handle<Value> argv[1] = { External::New((void *)raw) };
  return scope.Close(GitIndexTime::constructor_template->NewInstance(1, argv));
}

git_index_time *GitIndexTime::GetValue() {
  return this->raw;
}


Handle<Value> GitIndexTime::Seconds(const Arguments& args) {
  HandleScope scope;
    Handle<Value> to;

  git_time_t seconds =
    ObjectWrap::Unwrap<GitIndexTime>(args.This())->GetValue()->seconds;

    to = Uint32::New(seconds);
  return scope.Close(to);
}

Handle<Value> GitIndexTime::Nanoseconds(const Arguments& args) {
  HandleScope scope;
    Handle<Value> to;

  unsigned int nanoseconds =
    ObjectWrap::Unwrap<GitIndexTime>(args.This())->GetValue()->nanoseconds;

    to = Uint32::New(nanoseconds);
  return scope.Close(to);
}

Persistent<Function> GitIndexTime::constructor_template;

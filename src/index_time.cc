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
  NanScope();

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanSymbol("IndexTime"));

  NODE_SET_PROTOTYPE_METHOD(tpl, "seconds", Seconds);
  NODE_SET_PROTOTYPE_METHOD(tpl, "nanoseconds", Nanoseconds);

  NanAssignPersistent(FunctionTemplate, constructor_template, tpl);
  target->Set(String::NewSymbol("IndexTime"), tpl->GetFunction());
}

NAN_METHOD(GitIndexTime::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError(String::New("git_index_time is required."));
  }

  GitIndexTime* object = new GitIndexTime((git_index_time *) External::Cast(*args[0])->Value());
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitIndexTime::New(void *raw) {
  NanScope();
  Handle<Value> argv[1] = { External::New((void *)raw) };
  Local<Object> instance;
  Local<FunctionTemplate> constructorHandle = NanPersistentToLocal(constructor_template);
  instance = constructorHandle->GetFunction()->NewInstance(1, argv);
  return scope.Close(instance);
}

git_index_time *GitIndexTime::GetValue() {
  return this->raw;
}

NAN_METHOD(GitIndexTime::Seconds) {
  NanScope();
    Handle<Value> to;

  git_time_t seconds =
    ObjectWrap::Unwrap<GitIndexTime>(args.This())->GetValue()->seconds;

    to = Uint32::New(seconds);
  NanReturnValue(to);
}

NAN_METHOD(GitIndexTime::Nanoseconds) {
  NanScope();
    Handle<Value> to;

  unsigned int nanoseconds =
    ObjectWrap::Unwrap<GitIndexTime>(args.This())->GetValue()->nanoseconds;

    to = Uint32::New(nanoseconds);
  NanReturnValue(to);
}

Persistent<FunctionTemplate> GitIndexTime::constructor_template;

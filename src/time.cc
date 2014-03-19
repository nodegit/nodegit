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
  NanScope();

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanSymbol("Time"));
  
  NODE_SET_PROTOTYPE_METHOD(tpl, "time", Time);
  NODE_SET_PROTOTYPE_METHOD(tpl, "offset", Offset);

  NanAssignPersistent(FunctionTemplate, constructor_template, tpl);
  target->Set(String::NewSymbol("Time"), tpl->GetFunction());
}

NAN_METHOD(GitTime::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError(String::New("git_time is required."));
  }

  GitTime* object = new GitTime((git_time *) External::Cast(*args[0])->Value());
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitTime::New(void *raw) {
  NanScope();
  Handle<Value> argv[1] = { External::New((void *)raw) };
  Local<Object> instance;
  Local<FunctionTemplate> constructorHandle = NanPersistentToLocal(constructor_template);
  instance = constructorHandle->GetFunction()->NewInstance(1, argv);
  return scope.Close(instance);
}

git_time *GitTime::GetValue() {
  return this->raw;
}

NAN_METHOD(GitTime::Time) {
  NanScope();
    Handle<Value> to;

  git_time_t time =
    ObjectWrap::Unwrap<GitTime>(args.This())->GetValue()->time;

    to = Integer::New(time);
  NanReturnValue(to);
}

NAN_METHOD(GitTime::Offset) {
  NanScope();
    Handle<Value> to;

  int offset =
    ObjectWrap::Unwrap<GitTime>(args.This())->GetValue()->offset;

    to = Int32::New(offset);
  NanReturnValue(to);
}

Persistent<FunctionTemplate> GitTime::constructor_template;

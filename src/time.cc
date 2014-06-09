/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <nan.h>
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

  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanNew<String>("Time"));


  NODE_SET_PROTOTYPE_METHOD(tpl, "time", Time);
  NODE_SET_PROTOTYPE_METHOD(tpl, "offset", Offset);

  Local<Function> _constructor_template = tpl->GetFunction();
  NanAssignPersistent(constructor_template, _constructor_template);
  target->Set(NanNew<String>("Time"), _constructor_template);
}

NAN_METHOD(GitTime::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError("git_time is required.");
  }
  GitTime* object = new GitTime(static_cast<git_time *>(Handle<External>::Cast(args[0])->Value()));
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitTime::New(void *raw) {
  NanEscapableScope();
  Handle<Value> argv[1] = { NanNew<External>((void *)raw) };
  return NanEscapeScope(NanNew<Function>(GitTime::constructor_template)->NewInstance(1, argv));
}

git_time *GitTime::GetValue() {
  return this->raw;
}


NAN_METHOD(GitTime::Time) {
  NanScope();
    Handle<Value> to;

  git_time_t time =
    ObjectWrap::Unwrap<GitTime>(args.This())->GetValue()->time;

    to = NanNew<Integer>(time);
  NanReturnValue(to);
}

NAN_METHOD(GitTime::Offset) {
  NanScope();
    Handle<Value> to;

  int offset =
    ObjectWrap::Unwrap<GitTime>(args.This())->GetValue()->offset;

    to = NanNew<Int32>((int32_t)offset);
  NanReturnValue(to);
}

Persistent<Function> GitTime::constructor_template;

/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <nan.h>
#include <string.h>

#include "git2.h"

#include "../include/functions/copy.h"

#include "../include/refdb.h"

using namespace v8;
using namespace node;

GitRefDb::GitRefDb(git_refdb *raw) {
  this->raw = raw;
}

GitRefDb::~GitRefDb() {
  free(this->raw);
}

void GitRefDb::Initialize(Handle<v8::Object> target) {
  NanScope();

  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanNew<String>("RefDb"));



  Local<Function> _constructor_template = tpl->GetFunction();
  NanAssignPersistent(constructor_template, _constructor_template);
  target->Set(NanNew<String>("RefDb"), _constructor_template);
}

NAN_METHOD(GitRefDb::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError("git_refdb is required.");
  }
  GitRefDb* object = new GitRefDb(static_cast<git_refdb *>(Handle<External>::Cast(args[0])->Value()));
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitRefDb::New(void *raw) {
  NanEscapableScope();
  Handle<Value> argv[1] = { NanNew<External>((void *)raw) };
  return NanEscapeScope(NanNew<Function>(GitRefDb::constructor_template)->NewInstance(1, argv));
}

git_refdb *GitRefDb::GetValue() {
  return this->raw;
}


Persistent<Function> GitRefDb::constructor_template;

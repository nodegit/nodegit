/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <nan.h>
#include <string.h>

#include "git2.h"

#include "../include/functions/copy.h"

#include "../include/clone_options.h"

using namespace v8;
using namespace node;

GitCloneOptions::GitCloneOptions(git_clone_options *raw) {
  this->raw = raw;
}

GitCloneOptions::~GitCloneOptions() {
  free(this->raw);
}

void GitCloneOptions::Initialize(Handle<v8::Object> target) {
  NanScope();

  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanNew<String>("CloneOptions"));



  Local<Function> _constructor_template = tpl->GetFunction();
  NanAssignPersistent(constructor_template, _constructor_template);
  target->Set(NanNew<String>("CloneOptions"), _constructor_template);
}

NAN_METHOD(GitCloneOptions::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError("git_clone_options is required.");
  }
  GitCloneOptions* object = new GitCloneOptions(static_cast<git_clone_options *>(Handle<External>::Cast(args[0])->Value()));
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitCloneOptions::New(void *raw) {
  NanEscapableScope();
  Handle<Value> argv[1] = { NanNew<External>((void *)raw) };
  return NanEscapeScope(NanNew<Function>(GitCloneOptions::constructor_template)->NewInstance(1, argv));
}

git_clone_options *GitCloneOptions::GetValue() {
  return this->raw;
}


Persistent<Function> GitCloneOptions::constructor_template;

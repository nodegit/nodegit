/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <nan.h>
#include <string.h>

#include "git2.h"

#include "../include/functions/copy.h"

#include "../include/diff_find_options.h"

using namespace v8;
using namespace node;

GitDiffFindOptions::GitDiffFindOptions(git_diff_find_options *raw) {
  this->raw = raw;
}

GitDiffFindOptions::~GitDiffFindOptions() {
  free(this->raw);
}

void GitDiffFindOptions::Initialize(Handle<v8::Object> target) {
  NanScope();

  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanNew<String>("DiffFindOptions"));



  Local<Function> _constructor_template = tpl->GetFunction();
  NanAssignPersistent(constructor_template, _constructor_template);
  target->Set(NanNew<String>("DiffFindOptions"), _constructor_template);
}

NAN_METHOD(GitDiffFindOptions::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError("git_diff_find_options is required.");
  }
  GitDiffFindOptions* object = new GitDiffFindOptions(static_cast<git_diff_find_options *>(Handle<External>::Cast(args[0])->Value()));
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitDiffFindOptions::New(void *raw) {
  NanEscapableScope();
  Handle<Value> argv[1] = { NanNew<External>((void *)raw) };
  return NanEscapeScope(NanNew<Function>(GitDiffFindOptions::constructor_template)->NewInstance(1, argv));
}

git_diff_find_options *GitDiffFindOptions::GetValue() {
  return this->raw;
}


Persistent<Function> GitDiffFindOptions::constructor_template;

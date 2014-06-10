/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <nan.h>
#include <string.h>

#include "git2.h"

#include "../include/functions/copy.h"

#include "../include/delta.h"
#include "../include/diff_file.h"

using namespace v8;
using namespace node;

GitDelta::GitDelta(git_diff_delta *raw) {
  this->raw = raw;
}

GitDelta::~GitDelta() {
  free(this->raw);
}

void GitDelta::Initialize(Handle<v8::Object> target) {
  NanScope();

  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanNew<String>("Delta"));


  NODE_SET_PROTOTYPE_METHOD(tpl, "oldFile", OldFile);
  NODE_SET_PROTOTYPE_METHOD(tpl, "newFile", NewFile);
  NODE_SET_PROTOTYPE_METHOD(tpl, "status", Status);
  NODE_SET_PROTOTYPE_METHOD(tpl, "similarity", Similarity);
  NODE_SET_PROTOTYPE_METHOD(tpl, "flags", Flags);

  Local<Function> _constructor_template = tpl->GetFunction();
  NanAssignPersistent(constructor_template, _constructor_template);
  target->Set(NanNew<String>("Delta"), _constructor_template);
}

NAN_METHOD(GitDelta::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError("git_diff_delta is required.");
  }
  GitDelta* object = new GitDelta(static_cast<git_diff_delta *>(Handle<External>::Cast(args[0])->Value()));
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitDelta::New(void *raw) {
  NanEscapableScope();
  Handle<Value> argv[1] = { NanNew<External>((void *)raw) };
  return NanEscapeScope(NanNew<Function>(GitDelta::constructor_template)->NewInstance(1, argv));
}

git_diff_delta *GitDelta::GetValue() {
  return this->raw;
}


NAN_METHOD(GitDelta::OldFile) {
  NanScope();
    Handle<Value> to;

  git_diff_file *old_file =
    &ObjectWrap::Unwrap<GitDelta>(args.This())->GetValue()->old_file;

    if (old_file != NULL) {
    old_file = (git_diff_file *)git_diff_file_dup(old_file);
  }
  if (old_file != NULL) {
    to = GitDiffFile::New((void *)old_file);
  } else {
    to = NanNull();
  }
  NanReturnValue(to);
}

NAN_METHOD(GitDelta::NewFile) {
  NanScope();
    Handle<Value> to;

  git_diff_file *new_file =
    &ObjectWrap::Unwrap<GitDelta>(args.This())->GetValue()->new_file;

    if (new_file != NULL) {
    new_file = (git_diff_file *)git_diff_file_dup(new_file);
  }
  if (new_file != NULL) {
    to = GitDiffFile::New((void *)new_file);
  } else {
    to = NanNull();
  }
  NanReturnValue(to);
}

NAN_METHOD(GitDelta::Status) {
  NanScope();
    Handle<Value> to;

  git_delta_t status =
    ObjectWrap::Unwrap<GitDelta>(args.This())->GetValue()->status;

    to = NanNew<Integer>(status);
  NanReturnValue(to);
}

NAN_METHOD(GitDelta::Similarity) {
  NanScope();
    Handle<Value> to;

  uint32_t similarity =
    ObjectWrap::Unwrap<GitDelta>(args.This())->GetValue()->similarity;

    to = NanNew<Integer>(similarity);
  NanReturnValue(to);
}

NAN_METHOD(GitDelta::Flags) {
  NanScope();
    Handle<Value> to;

  uint32_t flags =
    ObjectWrap::Unwrap<GitDelta>(args.This())->GetValue()->flags;

    to = NanNew<Integer>(flags);
  NanReturnValue(to);
}

Persistent<Function> GitDelta::constructor_template;

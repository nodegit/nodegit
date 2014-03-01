/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
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

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanSymbol("Delta"));

  NODE_SET_PROTOTYPE_METHOD(tpl, "oldFile", OldFile);
  NODE_SET_PROTOTYPE_METHOD(tpl, "newFile", NewFile);
  NODE_SET_PROTOTYPE_METHOD(tpl, "status", Status);
  NODE_SET_PROTOTYPE_METHOD(tpl, "similarity", Similarity);
  NODE_SET_PROTOTYPE_METHOD(tpl, "flags", Flags);

  NanAssignPersistent(FunctionTemplate, constructor_template, tpl);
  target->Set(String::NewSymbol("Delta"), tpl->GetFunction());
}

NAN_METHOD(GitDelta::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError(String::New("git_diff_delta is required."));
  }

  GitDelta* object = new GitDelta((git_diff_delta *) External::Cast(*args[0])->Value());
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitDelta::New(void *raw) {
  NanScope();
  Handle<Value> argv[1] = { External::New((void *)raw) };
  Local<Object> instance;
  Local<FunctionTemplate> constructorHandle = NanPersistentToLocal(constructor_template);
  instance = constructorHandle->GetFunction()->NewInstance(1, argv);
  return scope.Close(instance);
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
    to = Null();
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
    to = Null();
  }
  NanReturnValue(to);
}

NAN_METHOD(GitDelta::Status) {
  NanScope();
    Handle<Value> to;

  git_delta_t status =
    ObjectWrap::Unwrap<GitDelta>(args.This())->GetValue()->status;

    to = Integer::New(status);
  NanReturnValue(to);
}

NAN_METHOD(GitDelta::Similarity) {
  NanScope();
    Handle<Value> to;

  uint32_t similarity =
    ObjectWrap::Unwrap<GitDelta>(args.This())->GetValue()->similarity;

    to = Integer::New(similarity);
  NanReturnValue(to);
}

NAN_METHOD(GitDelta::Flags) {
  NanScope();
    Handle<Value> to;

  uint32_t flags =
    ObjectWrap::Unwrap<GitDelta>(args.This())->GetValue()->flags;

    to = Integer::New(flags);
  NanReturnValue(to);
}

Persistent<FunctionTemplate> GitDelta::constructor_template;

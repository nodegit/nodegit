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
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Delta"));


  NODE_SET_PROTOTYPE_METHOD(tpl, "oldFile", OldFile);
  NODE_SET_PROTOTYPE_METHOD(tpl, "newFile", NewFile);
  NODE_SET_PROTOTYPE_METHOD(tpl, "status", Status);
  NODE_SET_PROTOTYPE_METHOD(tpl, "similarity", Similarity);
  NODE_SET_PROTOTYPE_METHOD(tpl, "flags", Flags);

  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Delta"), constructor_template);
}

Handle<Value> GitDelta::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return ThrowException(Exception::Error(String::New("git_diff_delta is required.")));
  }

  GitDelta* object = new GitDelta((git_diff_delta *) External::Unwrap(args[0]));
  object->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> GitDelta::New(void *raw) {
  HandleScope scope;
  Handle<Value> argv[1] = { External::New((void *)raw) };
  return scope.Close(GitDelta::constructor_template->NewInstance(1, argv));
}

git_diff_delta *GitDelta::GetValue() {
  return this->raw;
}


Handle<Value> GitDelta::OldFile(const Arguments& args) {
  HandleScope scope;
    Handle<Value> to;

  git_diff_file *old_file =
    &ObjectWrap::Unwrap<GitDelta>(args.This())->GetValue()->old_file;

    old_file = (git_diff_file *)git_diff_file_dup(old_file);
  to = GitDiffFile::New((void *)old_file);
  return scope.Close(to);
}

Handle<Value> GitDelta::NewFile(const Arguments& args) {
  HandleScope scope;
    Handle<Value> to;

  git_diff_file *new_file =
    &ObjectWrap::Unwrap<GitDelta>(args.This())->GetValue()->new_file;

    new_file = (git_diff_file *)git_diff_file_dup(new_file);
  to = GitDiffFile::New((void *)new_file);
  return scope.Close(to);
}

Handle<Value> GitDelta::Status(const Arguments& args) {
  HandleScope scope;
    Handle<Value> to;

  git_delta_t status =
    ObjectWrap::Unwrap<GitDelta>(args.This())->GetValue()->status;

    to = Integer::New(status);
  return scope.Close(to);
}

Handle<Value> GitDelta::Similarity(const Arguments& args) {
  HandleScope scope;
    Handle<Value> to;

  uint32_t similarity =
    ObjectWrap::Unwrap<GitDelta>(args.This())->GetValue()->similarity;

    to = Integer::New(similarity);
  return scope.Close(to);
}

Handle<Value> GitDelta::Flags(const Arguments& args) {
  HandleScope scope;
    Handle<Value> to;

  uint32_t flags =
    ObjectWrap::Unwrap<GitDelta>(args.This())->GetValue()->flags;

    to = Integer::New(flags);
  return scope.Close(to);
}

Persistent<Function> GitDelta::constructor_template;

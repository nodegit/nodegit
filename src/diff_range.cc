/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/functions/copy.h"

#include "../include/diff_range.h"

using namespace v8;
using namespace node;

GitDiffRange::GitDiffRange(git_diff_range *raw) {
  this->raw = raw;
}

GitDiffRange::~GitDiffRange() {
  free(this->raw);
}

void GitDiffRange::Initialize(Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("DiffRange"));


  NODE_SET_PROTOTYPE_METHOD(tpl, "oldStart", OldStart);
  NODE_SET_PROTOTYPE_METHOD(tpl, "oldLines", OldLines);
  NODE_SET_PROTOTYPE_METHOD(tpl, "newStart", NewStart);
  NODE_SET_PROTOTYPE_METHOD(tpl, "newLines", NewLines);

  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("DiffRange"), constructor_template);
}

Handle<Value> GitDiffRange::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return ThrowException(Exception::Error(String::New("git_diff_range is required.")));
  }

  GitDiffRange* object = new GitDiffRange((git_diff_range *) External::Unwrap(args[0]));
  object->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> GitDiffRange::New(void *raw) {
  HandleScope scope;
  Handle<Value> argv[1] = { External::New((void *)raw) };
  return scope.Close(GitDiffRange::constructor_template->NewInstance(1, argv));
}

git_diff_range *GitDiffRange::GetValue() {
  return this->raw;
}


Handle<Value> GitDiffRange::OldStart(const Arguments& args) {
  HandleScope scope;
    Handle<Value> to;

  int old_start =
    ObjectWrap::Unwrap<GitDiffRange>(args.This())->GetValue()->old_start;

    to = Integer::New(old_start);
  return scope.Close(to);
}

Handle<Value> GitDiffRange::OldLines(const Arguments& args) {
  HandleScope scope;
    Handle<Value> to;

  int old_lines =
    ObjectWrap::Unwrap<GitDiffRange>(args.This())->GetValue()->old_lines;

    to = Integer::New(old_lines);
  return scope.Close(to);
}

Handle<Value> GitDiffRange::NewStart(const Arguments& args) {
  HandleScope scope;
    Handle<Value> to;

  int new_start =
    ObjectWrap::Unwrap<GitDiffRange>(args.This())->GetValue()->new_start;

    to = Integer::New(new_start);
  return scope.Close(to);
}

Handle<Value> GitDiffRange::NewLines(const Arguments& args) {
  HandleScope scope;
    Handle<Value> to;

  int new_lines =
    ObjectWrap::Unwrap<GitDiffRange>(args.This())->GetValue()->new_lines;

    to = Integer::New(new_lines);
  return scope.Close(to);
}

Persistent<Function> GitDiffRange::constructor_template;

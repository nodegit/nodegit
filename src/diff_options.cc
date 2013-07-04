/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/functions/copy.h"

#include "../include/diff_options.h"

using namespace v8;
using namespace node;

GitDiffOptions::GitDiffOptions(git_diff_options *raw) {
  this->raw = raw;
}

GitDiffOptions::~GitDiffOptions() {
  free(this->raw);
}

void GitDiffOptions::Initialize(Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("DiffOptions"));



  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("DiffOptions"), constructor_template);
}

Handle<Value> GitDiffOptions::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return ThrowException(Exception::Error(String::New("git_diff_options is required.")));
  }

  GitDiffOptions* object = new GitDiffOptions((git_diff_options *) External::Unwrap(args[0]));
  object->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> GitDiffOptions::New(void *raw) {
  HandleScope scope;
  Handle<Value> argv[1] = { External::New((void *)raw) };
  return scope.Close(GitDiffOptions::constructor_template->NewInstance(1, argv));
}

git_diff_options *GitDiffOptions::GetValue() {
  return this->raw;
}


Persistent<Function> GitDiffOptions::constructor_template;

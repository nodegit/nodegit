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
  NanScope();

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanSymbol("DiffOptions"));


  NanAssignPersistent(FunctionTemplate, constructor_template, tpl);
  target->Set(String::NewSymbol("DiffOptions"), tpl->GetFunction());
}

NAN_METHOD(GitDiffOptions::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError(String::New("git_diff_options is required."));
  }

  GitDiffOptions* object = new GitDiffOptions((git_diff_options *) External::Cast(*args[0])->Value());
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitDiffOptions::New(void *raw) {
  NanScope();
  Handle<Value> argv[1] = { External::New((void *)raw) };
  Local<Object> instance;
  Local<FunctionTemplate> constructorHandle = NanPersistentToLocal(constructor_template);
  instance = constructorHandle->GetFunction()->NewInstance(1, argv);
  return scope.Close(instance);
}

git_diff_options *GitDiffOptions::GetValue() {
  return this->raw;
}


Persistent<FunctionTemplate> GitDiffOptions::constructor_template;

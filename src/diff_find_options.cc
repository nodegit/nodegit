/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
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

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanSymbol("DiffFindOptions"));


  NanAssignPersistent(FunctionTemplate, constructor_template, tpl);
  target->Set(String::NewSymbol("DiffFindOptions"), tpl->GetFunction());
}

NAN_METHOD(GitDiffFindOptions::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError(String::New("git_diff_find_options is required."));
  }

  GitDiffFindOptions* object = new GitDiffFindOptions((git_diff_find_options *) External::Cast(*args[0])->Value());
  object->Wrap(args.This());

  NanReturnValue(args.This());
}


Handle<Value> GitDiffFindOptions::New(void *raw) {
  NanScope();
  Handle<Value> argv[1] = { External::New((void *)raw) };
  Local<Object> instance;
  Local<FunctionTemplate> constructorHandle = NanPersistentToLocal(constructor_template);
  instance = constructorHandle->GetFunction()->NewInstance(1, argv);
  return scope.Close(instance);
}

git_diff_find_options *GitDiffFindOptions::GetValue() {
  return this->raw;
}


Persistent<FunctionTemplate> GitDiffFindOptions::constructor_template;

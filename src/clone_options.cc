/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
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

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanSymbol("CloneOptions"));


  NanAssignPersistent(FunctionTemplate, constructor_template, tpl);
  target->Set(String::NewSymbol("CloneOptions"), tpl->GetFunction());
}

NAN_METHOD(GitCloneOptions::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError(String::New("git_clone_options is required."));
  }

  GitCloneOptions* object = new GitCloneOptions((git_clone_options *) External::Cast(*args[0])->Value());
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitCloneOptions::New(void *raw) {
  NanScope();
  Handle<Value> argv[1] = { External::New((void *)raw) };
  Local<Object> instance;
  Local<FunctionTemplate> constructorHandle = NanPersistentToLocal(constructor_template);
  instance = constructorHandle->GetFunction()->NewInstance(1, argv);
  return scope.Close(instance);
}

git_clone_options *GitCloneOptions::GetValue() {
  return this->raw;
}


Persistent<FunctionTemplate> GitCloneOptions::constructor_template;

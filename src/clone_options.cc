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
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("CloneOptions"));



  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("CloneOptions"), constructor_template);
}

Handle<Value> GitCloneOptions::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return ThrowException(Exception::Error(String::New("git_clone_options is required.")));
  }

  GitCloneOptions* object = new GitCloneOptions((git_clone_options *) External::Unwrap(args[0]));
  object->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> GitCloneOptions::New(void *raw) {
  HandleScope scope;
  Handle<Value> argv[1] = { External::New((void *)raw) };
  return scope.Close(GitCloneOptions::constructor_template->NewInstance(1, argv));
}

git_clone_options *GitCloneOptions::GetValue() {
  return this->raw;
}


Persistent<Function> GitCloneOptions::constructor_template;

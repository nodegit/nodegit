/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/functions/copy.h"

#include "../include/refdb.h"

using namespace v8;
using namespace node;

GitRefDb::GitRefDb(git_refdb *raw) {
  this->raw = raw;
}

GitRefDb::~GitRefDb() {
  free(this->raw);
}

void GitRefDb::Initialize(Handle<v8::Object> target) {
  NanScope();

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanSymbol("RefDb"));


  NanAssignPersistent(FunctionTemplate, constructor_template, tpl);
  target->Set(String::NewSymbol("RefDb"), tpl->GetFunction());
}

NAN_METHOD(GitRefDb::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError(String::New("git_refdb is required."));
  }

  GitRefDb* object = new GitRefDb((git_refdb *) External::Cast(*args[0])->Value());
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitRefDb::New(void *raw) {
  NanScope();
  Handle<Value> argv[1] = { External::New((void *)raw) };
  Local<Object> instance;
  Local<FunctionTemplate> constructorHandle = NanPersistentToLocal(constructor_template);
  instance = constructorHandle->GetFunction()->NewInstance(1, argv);
  return scope.Close(instance);
}

git_refdb *GitRefDb::GetValue() {
  return this->raw;
}


Persistent<FunctionTemplate> GitRefDb::constructor_template;

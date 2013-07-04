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
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("RefDb"));



  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("RefDb"), constructor_template);
}

Handle<Value> GitRefDb::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return ThrowException(Exception::Error(String::New("git_refdb is required.")));
  }

  GitRefDb* object = new GitRefDb((git_refdb *) External::Unwrap(args[0]));
  object->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> GitRefDb::New(void *raw) {
  HandleScope scope;
  Handle<Value> argv[1] = { External::New((void *)raw) };
  return scope.Close(GitRefDb::constructor_template->NewInstance(1, argv));
}

git_refdb *GitRefDb::GetValue() {
  return this->raw;
}


Persistent<Function> GitRefDb::constructor_template;

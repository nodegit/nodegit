/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/signature.h"
#include "../include/time.h"

#include "../include/functions/utilities.h"
#include "../include/functions/string.h"

using namespace v8;
using namespace node;

GitSignature::GitSignature(git_signature *raw) {
  this->raw = raw;
}

GitSignature::~GitSignature() {
}

void GitSignature::Initialize(Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Signature"));

  NODE_SET_METHOD(tpl, "now", Now);

  NODE_SET_PROTOTYPE_METHOD(tpl, "name", Name);
  NODE_SET_PROTOTYPE_METHOD(tpl, "email", Email);
  NODE_SET_PROTOTYPE_METHOD(tpl, "time", Time);

  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Signature"), constructor_template);
}

Handle<Value> GitSignature::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return ThrowException(Exception::Error(String::New("git_signature is required.")));
  }

  GitSignature* object = new GitSignature((git_signature *) External::Unwrap(args[0]));
  object->Wrap(args.This());

  return scope.Close(args.This());
}

git_signature *GitSignature::GetValue() {
  return this->raw;
}


Handle<Value> GitSignature::Now(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String name is required.")));
  }

  if (args.Length() == 1 || !args[1]->IsString()) {
    return ThrowException(Exception::Error(String::New("String email is required.")));
  }
  git_signature * out;

  int result = git_signature_now(

&
    out
, 

    stringArgToString(args[0]->ToString()).c_str()
, 

    stringArgToString(args[1]->ToString()).c_str()
  );

  if (result != GIT_OK) {
    return ThrowException(GitError::WrapError(giterr_last()));
  }

  // XXX need to copy object?
  Handle<Value> argv[1] = { External::New((void *)out) };
  return scope.Close(GitSignature::constructor_template->NewInstance(1, argv));
}

Handle<Value> GitSignature::Name(const Arguments& args) {
  HandleScope scope;
  const char * field = ObjectWrap::Unwrap<GitSignature>(args.This())->GetValue()->name;

  return scope.Close(String::New(field));
}
Handle<Value> GitSignature::Email(const Arguments& args) {
  HandleScope scope;
  const char * field = ObjectWrap::Unwrap<GitSignature>(args.This())->GetValue()->email;

  return scope.Close(String::New(field));
}
Handle<Value> GitSignature::Time(const Arguments& args) {
  HandleScope scope;
  git_time field = ObjectWrap::Unwrap<GitSignature>(args.This())->GetValue()->when;

  // XXX need to copy object?
  Handle<Value> argv[1] = { External::New((void *)&field) };
  return scope.Close(GitTime::constructor_template->NewInstance(1, argv));
}

Persistent<Function> GitSignature::constructor_template;

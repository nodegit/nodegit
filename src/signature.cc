/*
 * Copyright 2013, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * @author Michael Robinson @codeofinterest <mike@pagesofinterest.net>
 *
 * Dual licensed under the MIT and GPL licenses.
 */

#include <v8.h>
#include <node.h>

#include "../vendor/libgit2/include/git2.h"

#include "../include/repo.h"
#include "../include/sig.h"

using namespace v8;
using namespace node;

void GitSignature::Initialize (Handle<v8::Object> target) {
  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Signature"));

  NODE_SET_PROTOTYPE_METHOD(tpl, "duplicate", Duplicate);
  NODE_SET_PROTOTYPE_METHOD(tpl, "name", Name);
  NODE_SET_PROTOTYPE_METHOD(tpl, "email", Email);

  NODE_SET_PROTOTYPE_METHOD(tpl, "free", Free);

  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Signature"), constructor_template);
}

git_signature* GitSignature::GetValue() {
  return this->sig;
}

void GitSignature::SetValue(git_signature* sig) {
  this->sig = sig;
  this->name = sig->name;
  this->email = sig->email;
}

void GitSignature::New(const char *name, const char *email, time_t time, int offset) {
  git_signature_new(&this->sig, name, email, time, offset);
  //this->sig = git_signature_new(name, email, time, offset);
}

char* GitSignature::Name() {
  return this->name;
}

char* GitSignature::Email() {
  return this->email;
}

Handle<Value> GitSignature::New(const Arguments& args) {
  HandleScope scope;

  GitSignature *signature = new GitSignature();
  signature->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> GitSignature::Duplicate(const Arguments& args) {
  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("GitSignature is required and must be an Object.")));
  }

  Local<Object> duplicateSignature = GitSignature::constructor_template->NewInstance();
  GitSignature *duplicateSignatureInstance = ObjectWrap::Unwrap<GitSignature>(signature);

  GitSignature* signature = ObjectWrap::Unwrap<GitSignature>(args[0]->ToObject());

  duplicateSignatureInstance->SetValue(git_signature_dup(signature->GetValue()));

  return duplicateSignature;
}

Handle<Value> GitSignature::Free(const Arguments& args) {
  HandleScope scope;

  GitSignature *signature = ObjectWrap::Unwrap<GitSignature>(args.This());
  git_signature_free(this->signature);

  return Undefined();
}

Handle<Value> GitSignature::Name(const Arguments& args) {
  HandleScope scope;

  GitSignature *signature = ObjectWrap::Unwrap<GitSignature>(args.This());

  return String::New(signature->Name());
}

Handle<Value> GitSignature::Email(const Arguments& args) {
  HandleScope scope;

  GitSignature *signature = ObjectWrap::Unwrap<GitSignature>(args.This());

  return String::New(signature->Email());
}

Persistent<Function> GitSignature::constructor_template;

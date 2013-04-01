/*
 * Copyright 2013, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * @author Michael Robinson @codeofinterest <mike@pagesofinterest.net>
 *
 * Dual licensed under the MIT and GPL licenses.
 */

#include <v8.h>
#include <node.h>

#include "git2.h"

#include "../include/repo.h"
#include "../include/signature.h"

using namespace v8;
using namespace node;

void GitSignature::Initialize(Handle<v8::Object> target) {
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
  return this->signature;
}

void GitSignature::SetValue(git_signature* signature) {
  this->signature = signature;
}

Handle<Value> GitSignature::New(const Arguments& args) {
  HandleScope scope;

  GitSignature *signature = new GitSignature();
  signature->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> GitSignature::Free(const Arguments& args) {
  HandleScope scope;

  GitSignature *signature = ObjectWrap::Unwrap<GitSignature>(args.This());
  git_signature_free(signature->signature);
  signature->signature = NULL;

  return Undefined();
}

Handle<Value> GitSignature::Duplicate(const Arguments& args) {
  HandleScope scope;

  Local<Object> duplicateSignature = GitSignature::constructor_template->NewInstance();
  GitSignature *duplicateSignatureInstance = ObjectWrap::Unwrap<GitSignature>(duplicateSignature);

  GitSignature* signature = ObjectWrap::Unwrap<GitSignature>(args.This());
  duplicateSignatureInstance->SetValue(git_signature_dup(signature->GetValue()));

  return duplicateSignature;
}

Handle<Value> GitSignature::Name(const Arguments& args) {
  HandleScope scope;

  GitSignature *signature = ObjectWrap::Unwrap<GitSignature>(args.This());

  return scope.Close(String::New(signature->GetValue()->name));
}

Handle<Value> GitSignature::Email(const Arguments& args) {
  HandleScope scope;

  GitSignature *signature = ObjectWrap::Unwrap<GitSignature>(args.This());

  return scope.Close(String::New(signature->GetValue()->email));
}

Persistent<Function> GitSignature::constructor_template;

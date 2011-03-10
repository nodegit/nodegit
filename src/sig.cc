/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include "../vendor/libgit2/src/git2.h"

#include "repo.h"
#include "sig.h"

using namespace v8;
using namespace node;

void Sig::Initialize (Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(3);
  constructor_template->SetClassName(String::NewSymbol("Sig"));

  NODE_SET_PROTOTYPE_METHOD(constructor_template, "dup", Dup);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "free", Free);

  // FIXME: This is an irresponsible way to accomplish fetching properties from the struct
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "name", Name);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "email", Email);

  target->Set(String::NewSymbol("Sig"), constructor_template->GetFunction());
}

git_signature* Sig::GetValue() {
  return this->sig;
}

void Sig::SetValue(git_signature* sig) {
  this->sig = sig;
  this->name = sig->name;
  this->email = sig->email;
}

void Sig::New(const char *name, const char *email, time_t time, int offset) {
  this->sig = git_signature_new(name, email, time, offset);
}

git_signature* Sig::Dup() {
  return git_signature_dup(this->sig);
}

void Sig::Free() {
  git_signature_free(this->sig);
}

char* Sig::Name() {
  return this->name;
}

char* Sig::Email() {
  return this->email;
}

Handle<Value> Sig::New(const Arguments& args) {
  HandleScope scope;

  Sig *sig = new Sig();
  sig->Wrap(args.This());

  return args.This();
}

Handle<Value> Sig::Dup(const Arguments& args) {
  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Signature is required and must be an Object.")));
  }

  Sig* sig = ObjectWrap::Unwrap<Sig>(args[0]->ToObject());
  sig->SetValue(sig->Dup());

  return Undefined();
}

Handle<Value> Sig::Free(const Arguments& args) {
  HandleScope scope;

  Sig *sig = ObjectWrap::Unwrap<Sig>(args.This());
  sig->Free();

  return Undefined();
}

Handle<Value> Sig::Name(const Arguments& args) {
  HandleScope scope;

  Sig *sig = ObjectWrap::Unwrap<Sig>(args.This());

  return String::New(sig->Name());
}

Handle<Value> Sig::Email(const Arguments& args) {
  HandleScope scope;

  Sig *sig = ObjectWrap::Unwrap<Sig>(args.This());

  return String::New(sig->Email());
}
Persistent<FunctionTemplate> Sig::constructor_template;

/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include <git2.h>

#include "repo.h"
#include "sig.h"

using namespace v8;
using namespace node;

void Sig::Initialize (Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("Sig"));

  target->Set(String::NewSymbol("Sig"), constructor_template->GetFunction());
}

git_signature* Sig::GetValue() {
  return this->sig;
}

void Sig::SetValue(git_signature* sig) {
  this->sig = sig;
}

void Sig::New(const char *name, const char *email, time_t time, int offset) {
  this->sig = git_signature_new(name, email, time, offset);
}

void Sig::Free() {
  git_signature_free(this->sig);
}

Handle<Value> Sig::New(const Arguments& args) {
  HandleScope scope;

  Sig *sig = new Sig();

  //if(args.Length() == 0 || !args[0]->IsObject()) {
  //  return ThrowException(Exception::Error(String::New("Repo is required and must be an Object.")));
  //}

  //Repo *repo = ObjectWrap::Unwrap<Repo>(args[0]->ToObject());
  //sig->New((git_repository *)repo);
    
  sig->Wrap(args.This());

  return args.This();
}
Persistent<FunctionTemplate> Sig::constructor_template;

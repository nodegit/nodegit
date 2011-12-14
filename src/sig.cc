/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#include <v8.h>
#include <node.h>

#include "../vendor/libgit2/include/git2.h"

#include "../include/repo.h"
#include "../include/sig.h"

using namespace v8;
using namespace node;

void GitSig::Initialize (Handle<v8::Object> target) {
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

git_signature* GitSig::GetValue() {
  return this->sig;
}

void GitSig::SetValue(git_signature* sig) {
  this->sig = sig;
  this->name = sig->name;
  this->email = sig->email;
}

void GitSig::New(const char *name, const char *email, time_t time, int offset) {
  git_signature_new(&this->sig, name, email, time, offset);
  //this->sig = git_signature_new(name, email, time, offset);
}

git_signature* GitSig::Dup() {
  return git_signature_dup(this->sig);
}

void GitSig::Free() {
  git_signature_free(this->sig);
}

char* GitSig::Name() {
  return this->name;
}

char* GitSig::Email() {
  return this->email;
}

Handle<Value> GitSig::New(const Arguments& args) {
  HandleScope scope;

  GitSig *sig = new GitSig();
  sig->Wrap(args.This());

  return scope.Close( args.This() );
}

Handle<Value> GitSig::Dup(const Arguments& args) {
  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("GitSignature is required and must be an Object.")));
  }

  GitSig* sig = ObjectWrap::Unwrap<GitSig>(args[0]->ToObject());
  sig->SetValue(sig->Dup());

  return scope.Close( Undefined() );
}

Handle<Value> GitSig::Free(const Arguments& args) {
  HandleScope scope;

  GitSig *sig = ObjectWrap::Unwrap<GitSig>(args.This());
  sig->Free();

  return scope.Close( Undefined() );
}

Handle<Value> GitSig::Name(const Arguments& args) {
  HandleScope scope;

  GitSig *sig = ObjectWrap::Unwrap<GitSig>(args.This());

  return scope.Close( String::New(sig->Name()) );
}

Handle<Value> GitSig::Email(const Arguments& args) {
  HandleScope scope;

  GitSig *sig = ObjectWrap::Unwrap<GitSig>(args.This());

  return scope.Close( String::New(sig->Email()) );
}
Persistent<FunctionTemplate> GitSig::constructor_template;

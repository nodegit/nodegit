/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include <git2.h>

#include "repo.h"
#include "blob.h"

using namespace v8;
using namespace node;

void Blob::Initialize (Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("Blob"));

  target->Set(String::NewSymbol("Blob"), constructor_template->GetFunction());
}

git_blob* Blob::GetValue() {
  return this->blob;
}

void Blob::SetValue(git_blob* blob) {
  this->blob = blob;
}

int Blob::New(git_repository* repo) {
  return git_blob_new(&this->blob, repo);
}

Handle<Value> Blob::New(const Arguments& args) {
  HandleScope scope;

  Blob *blob = new Blob();

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repo is required and must be an Object.")));
  }

  Repo *repo = ObjectWrapper::Unwrap<Repo>(args[0]);
  int err = blob->New(repo);
    
  blob->Wrap(args.This());

  return args.This();
}
Persistent<FunctionTemplate> Blob::constructor_template;

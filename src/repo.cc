/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#include <v8.h>
#include <node.h>
#include <node_events.h>
#include "repo.h"

using namespace v8;
using namespace node;

void Repo::Init (Handle<Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("Repo"));

  NODE_SET_PROTOTYPE_METHOD(constructor_template, "open", Open);
  NODE_SET_PROTOTYPE_METHOD(s_ct, "free", Free);

  target->Set(String::NewSymbol("Repo"), constructor_template->GetFunction());
}

int Repo::Open (const char* path) {
  return git_repository_open(&this->repo, path);
}

void Repo::Free () {
  git_repository_free(this->repo);
}

Handle<Value> Repo::New (const Arguments& args) {
  HandleScope scope;

  Repo *repo = new Repo();
  repo->Wrap(args.This());

  return args.This();
}

Handle<Value> Repo::Open (const Arguments& args) {
  Repo *repo = ObjectWrap::Unwrap<Repo>(args.This());
  Local<Function> callback;

  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsString())
    return ThrowException(Exception::Error(String::New("Path is required and must be a String.")));

  if (args.Length() != 2 || !args[1]->IsFunction())
    return ThrowException(Exception::Error(String::New("Callback must be a Function.")));

  callback = Local<Function>::Cast(args[1]);

  open_request *ar = new open_request();
  ar->repo = repo;
  ar->path = Persistent<Value>::New( args[0] );
  ar->callback = Persistent<Function>::New(callback);

  repo->Ref();

  eio_custom(AsyncOpen, EIO_PRI_DEFAULT, AsyncOpenComplete, ar);
  ev_ref(EV_DEFAULT_UC);

  return Undefined();
}

int Repo::AsyncOpen(eio_req *req) {
  open_request *ar = static_cast<open_request *>(req->data);

  String::Utf8Value path(ar->path);
  ar->err = Persistent<Value>::New( Integer::New(ar->repo->Open(*path)) );

  return 0;
}

int Repo::AsyncOpenComplete(eio_req *req) {
  HandleScope scope;

  open_request *ar = static_cast<open_request *>(req->data);
  ev_unref(EV_DEFAULT_UC);
  ar->repo->Unref();

  Local<Value> argv[2];
  argv[0] = Number::Cast(*ar->err);
  argv[1] = String::Cast(*ar->path);

  TryCatch try_catch;

  ar->callback->Call(Context::GetCurrent()->Global(), 2, argv);

  if (try_catch.HasCaught())
    FatalException(try_catch);
    
  ar->err.Dispose();
  ar->callback.Dispose();
  ar->path.Dispose();

  delete ar;

  return 0;
}

Handle<Value> Repo::Free (const Arguments& args) {
  Repo *repo = ObjectWrap::Unwrap<Repo>(args.This());

  HandleScope scope;

  repo->Free();

  return Undefined();
}

extern "C" void init (Handle<Object> target) {
  HandleScope scope;

  Repo::Init(target);
}

Persistent<FunctionTemplate> Repo::constructor_template;

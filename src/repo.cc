/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#include <v8.h>
#include <node.h>
#include <string>

#include "../vendor/libgit2/include/git2.h"

#include "../include/object.h"
#include "../include/repo.h"
#include "../include/commit.h"

using namespace v8;
using namespace node;

void GitRepo::Initialize(Handle<Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("Repo"));

  NODE_SET_PROTOTYPE_METHOD(constructor_template, "open", Open);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "lookup", Lookup);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "free", Free);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "init", Init);

  target->Set(String::NewSymbol("Repo"), constructor_template->GetFunction());
}

git_repository* GitRepo::GetValue() {
    return this->repo;
}

void GitRepo::SetValue(git_repository* repo) {
  this->repo = repo;
}

int GitRepo::Open(const char* path) {
  return git_repository_open(&this->repo, path);
}

void GitRepo::Free() {
  git_repository_free(this->repo);
}

int GitRepo::Init(const char* path, bool is_bare) {
  git_repository* repo_;
  int err = git_repository_init(&repo_, path, is_bare);

  if(err == 0) {
    this->repo = *&repo_;
  }

  return err;
}

Handle<Value> GitRepo::New(const Arguments& args) {
  HandleScope scope;

  GitRepo *repo = new GitRepo();
  repo->Wrap(args.This());

  return scope.Close( args.This() );
}

Handle<Value> GitRepo::Open(const Arguments& args) {
  HandleScope scope;

  GitRepo *repo = ObjectWrap::Unwrap<GitRepo>(args.This());
  Local<Function> callback;

  if(args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("Path is required and must be a String.")));
  }

  if(args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  callback = Local<Function>::Cast(args[1]);

  open_request *ar = new open_request();
  ar->repo = repo;

  String::Utf8Value path(args[0]);
  ar->path = *path;

  ar->callback = Persistent<Function>::New(callback);

  repo->Ref();

  eio_custom(EIO_Open, EIO_PRI_DEFAULT, EIO_AfterOpen, ar);
  ev_ref(EV_DEFAULT_UC);

  return scope.Close( Undefined() );
}

void GitRepo::EIO_Open(eio_req *req) {
  open_request *ar = static_cast<open_request *>(req->data);

  ar->err = ar->repo->Open(ar->path.c_str());

}

int GitRepo::EIO_AfterOpen(eio_req *req) {
  HandleScope scope;

  open_request *ar = static_cast<open_request *>(req->data);
  ev_unref(EV_DEFAULT_UC);
  ar->repo->Unref();

  Local<Value> argv[1];
  argv[0] = Integer::New(ar->err);

  TryCatch try_catch;

  ar->callback->Call(Context::GetCurrent()->Global(), 1, argv);

  if(try_catch.HasCaught())
    FatalException(try_catch);
    
  ar->callback.Dispose();

  delete ar;

  return 0;
}

Handle<Value> GitRepo::Lookup(const Arguments& args) {
  HandleScope scope;

  GitRepo *repo = ObjectWrap::Unwrap<GitRepo>(args.This());
  Local<Function> callback;

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Object is required and must be a Object.")));
  }

  if(args.Length() == 1 || !args[1]->IsObject()) {
    return ThrowException(Exception::Error(String::New("GitRepo is required and must be a Object.")));
  }

  if(args.Length() == 2 || !args[2]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid is required and must be a Object.")));
  }

  if(args.Length() == 3 || !args[3]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  callback = Local<Function>::Cast(args[3]);

  lookup_request *ar = new lookup_request();
  ar->repo = repo;
  ar->callback = Persistent<Function>::New(callback);

  repo->Ref();

  //eio_custom(EIO_LookupRef, EIO_PRI_DEFAULT, EIO_AfterLookupRef, ar);
  //ev_ref(EV_DEFAULT_UC);

  return scope.Close( Undefined() );
}

int GitRepo::EIO_Lookup(eio_req *req) {
  //lookup_request *ar = static_cast<lookup_request *>(req->data);
  //
  //String::Utf8Value name(ar->name);
  //git_reference *ref;
  //
  //int err = ar->repo->LookupRef((git_reference **)ref, *name);
  //ar->err = Persistent<Value>::New(Integer::New(err));
  //
  //if(Int32::Cast(*ar->err)->Value() == 0) {
  //  ar->ref->SetValue(*&ref);
  //}
  //
  //return 0;
}

int GitRepo::EIO_AfterLookup(eio_req *req) {
  //HandleScope scope;

  //lookup_request *ar = static_cast<lookupref_request *>(req->data);
  //ev_unref(EV_DEFAULT_UC);
  //ar->repo->Unref();

  //Local<Value> argv[1];
  //argv[0] = Number::Cast(*ar->err);

  //TryCatch try_catch;

  //ar->callback->Call(Context::GetCurrent()->Global(), 1, argv);

  //if(try_catch.HasCaught())
  //  FatalException(try_catch);
  //  
  //ar->err.Dispose();
  //ar->name.Dispose();
  //ar->callback.Dispose();

  //delete ar;

  //return 0;
}

Handle<Value> GitRepo::Free(const Arguments& args) {
  HandleScope scope;

  GitRepo *repo = ObjectWrap::Unwrap<GitRepo>(args.This());

  repo->Free();

  return scope.Close( Undefined() );
}

Handle<Value> GitRepo::Init(const Arguments& args) {
  HandleScope scope;

  GitRepo *repo = ObjectWrap::Unwrap<GitRepo>(args.This());
  Local<Function> callback;

  if(args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("path is required and must be a String.")));
  }

  if(args.Length() == 1 || !args[1]->IsBoolean()) {
    return ThrowException(Exception::Error(String::New("is_bare is required and must be a Boolean.")));
  }

  if(args.Length() == 2 || !args[2]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  callback = Local<Function>::Cast(args[2]);

  init_request *ar = new init_request();
  ar->repo = repo;

  String::Utf8Value path(args[0]);
  ar->path = *path;

  ar->is_bare = args[1]->ToBoolean()->Value();
  ar->callback = Persistent<Function>::New(callback);

  repo->Ref();

  eio_custom(EIO_Init, EIO_PRI_DEFAULT, EIO_AfterInit, ar);
  ev_ref(EV_DEFAULT_UC);

  return scope.Close( Undefined() );
}

void GitRepo::EIO_Init(eio_req *req) {
  init_request *ar = static_cast<init_request *>(req->data);

  ar->err = ar->repo->Init(ar->path.c_str(), ar->is_bare);

}

int GitRepo::EIO_AfterInit(eio_req *req) {
  HandleScope scope;

  init_request *ar = static_cast<init_request *>(req->data);
  ev_unref(EV_DEFAULT_UC);
  ar->repo->Unref();

  Local<Value> argv[2];
  argv[0] = Integer::New(ar->err);

  TryCatch try_catch;

  ar->callback->Call(Context::GetCurrent()->Global(), 1, argv);

  if(try_catch.HasCaught())
    FatalException(try_catch);
    
  ar->callback.Dispose();

  delete ar;

  return 0;
}
Persistent<FunctionTemplate> GitRepo::constructor_template;

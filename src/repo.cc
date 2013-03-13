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
#include "../include/error.h"

using namespace v8;
using namespace node;

void GitRepo::Initialize(Handle<Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Repo"));

  NODE_SET_PROTOTYPE_METHOD(tpl, "open", Open);
  NODE_SET_PROTOTYPE_METHOD(tpl, "lookup", Lookup);
  NODE_SET_PROTOTYPE_METHOD(tpl, "free", Free);
  NODE_SET_PROTOTYPE_METHOD(tpl, "init", Init);

  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Repo"), constructor_template);
}

git_repository* GitRepo::GetValue() {
    return this->repo;
}

void GitRepo::SetValue(git_repository* repo) {
  this->repo = repo;
}

void GitRepo::Free() {
  git_repository_free(this->repo);
}

Handle<Value> GitRepo::New(const Arguments& args) {
  HandleScope scope;

  GitRepo *repo = new GitRepo();
  repo->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> GitRepo::Open(const Arguments& args) {
  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("Path is required and must be a String.")));
  }

  if(args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  OpenBaton *baton = new OpenBaton();
  baton->request.data = baton;
  baton->error = NULL;
  String::Utf8Value path(args[0]);
  baton->path = *path;
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args.This());
  baton->repo->Ref();
  baton->rawRepo = baton->repo->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, OpenWork, OpenAfterWork);

  return Undefined();
}

void GitRepo::OpenWork(uv_work_t *req) {
  OpenBaton *baton = static_cast<OpenBaton *>(req->data);

  int returnCode = git_repository_open(&baton->rawRepo, baton->path.c_str());
  if (returnCode != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitRepo::OpenAfterWork(uv_work_t *req) {
  HandleScope scope;

  OpenBaton *baton = static_cast<OpenBaton *>(req->data);
  delete req;

  baton->repo->Unref();

  if (baton->error) {
    Local<Value> argv[1] = {
      GitError::WrapError(baton->error)
    };

    TryCatch try_catch;

    baton->callback->Call(Context::GetCurrent()->Global(), 1, argv);

    if (try_catch.HasCaught()) {
      node::FatalException(try_catch);
    }
  } else {

    baton->repo->SetValue(baton->rawRepo);

    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      baton->repo->handle_
    };

    TryCatch try_catch;
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
    if (try_catch.HasCaught()) {
      node::FatalException(try_catch);
    }
  }
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

void GitRepo::EIO_Lookup(uv_work_t *req) {
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

void GitRepo::EIO_AfterLookup(uv_work_t *req) {
  //HandleScope scope;

  //lookup_request *ar = static_cast<lookupref_request *>(req->data);
  // delete req;
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


  if(args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("path is required and must be a String.")));
  }

  if(args.Length() == 1 || !args[1]->IsBoolean()) {
    return ThrowException(Exception::Error(String::New("is_bare is required and must be a Boolean.")));
  }

  if(args.Length() == 2 || !args[2]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  InitBaton *baton = new InitBaton();
  baton->request.data = baton;
  baton->error = NULL;
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args.This());
  baton->repo->Ref();
  baton->rawRepo = baton->repo->GetValue();
  String::Utf8Value path(args[0]);
  baton->path = *path;
  baton->isBare = args[1]->ToBoolean()->Value();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[2]));

  uv_queue_work(uv_default_loop(), &baton->request, InitWork, InitAfterWork);

  return Undefined();
}

void GitRepo::InitWork(uv_work_t *req) {
  InitBaton *baton = static_cast<InitBaton *>(req->data);

  int returnCode = git_repository_init(&baton->rawRepo, baton->path.c_str(), baton->isBare);
  if (returnCode != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitRepo::InitAfterWork(uv_work_t *req) {
  HandleScope scope;

  InitBaton *baton = static_cast<InitBaton *>(req->data);
  delete req;
  baton->repo->Unref();

  Local<Value> argv[1];
  if (baton->error) {
    argv[0] = GitError::WrapError(baton->error);
  } else {
    argv[0] = Local<Value>::New(Null());
  }

  TryCatch try_catch;

  baton->callback->Call(Context::GetCurrent()->Global(), 1, argv);

  if (try_catch.HasCaught()) {
      node::FatalException(try_catch);
  }
}

Persistent<Function> GitRepo::constructor_template;

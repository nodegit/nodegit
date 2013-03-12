/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#include <v8.h>
#include <node.h>

#include "../vendor/libgit2/include/git2.h"

#include "../include/revwalk.h"
#include "../include/repo.h"
#include "../include/commit.h"
#include "../include/error.h"

using namespace v8;
using namespace node;

void GitRevWalk::Initialize(Handle<Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(New);

  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("RevWalk"));

  NODE_SET_PROTOTYPE_METHOD(constructor_template, "reset", Reset);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "push", Push);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "next", Next);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "free", Free);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "repository", Repository);

  Local<Object> sort = Object::New();

  sort->Set(String::New("NONE"), Integer::New(0));
  sort->Set(String::New("TOPOLOGICAL"), Integer::New(1));
  sort->Set(String::New("TIME"), Integer::New(2));
  sort->Set(String::New("REVERSE"), Integer::New(4));

  constructor_template->Set(String::New("sort"), sort);

  target->Set(String::NewSymbol("RevWalk"), constructor_template->GetFunction());
}

git_revwalk* GitRevWalk::GetValue() {
  return this->revwalk;
}

void GitRevWalk::SetValue(git_revwalk* revwalk) {
  this->revwalk = revwalk;
}

int GitRevWalk::New(git_repository* repo) {
  this->repo = repo;

  return git_revwalk_new(&this->revwalk, this->repo);
}

void GitRevWalk::Reset() {
  git_revwalk_reset(this->revwalk);
}

void GitRevWalk::Free() {
  git_revwalk_free(this->revwalk);
}

git_repository* GitRevWalk::Repository() {
	return git_revwalk_repository(this->revwalk);
}

Handle<Value> GitRevWalk::New(const Arguments& args) {
  HandleScope scope;

  GitRevWalk *revwalk = new GitRevWalk();

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repo is required and must be an Object.")));
  }

  GitRepo *repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject());
  revwalk->New(repo->GetValue());

  revwalk->Wrap(args.This());

  return scope.Close( args.This() );
}

Handle<Value> GitRevWalk::Reset(const Arguments& args) {
  HandleScope scope;

  GitRevWalk *revwalk = ObjectWrap::Unwrap<GitRevWalk>(args.This());

  revwalk->Reset();

  return scope.Close( Undefined() );
}

Handle<Value> GitRevWalk::Push(const Arguments& args) {
  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid is required and must be an Object.")));
  }

  if(args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  PushBaton* baton = new PushBaton();

  baton->request.data = baton;
  baton->revwalk = ObjectWrap::Unwrap<GitRevWalk>(args.This())->GetValue();
  baton->oid = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, PushWork, PushAfterWork);

  return Undefined();
}

void GitRevWalk::PushWork(uv_work_t *req) {
  PushBaton *baton = static_cast<PushBaton *>(req->data);

  git_revwalk_sorting(baton->revwalk, GIT_SORT_TIME | GIT_SORT_REVERSE);

  int returnCode = git_revwalk_push(baton->revwalk, &baton->oid);
  if (returnCode) {
    baton->error = giterr_last();
  }
}

void GitRevWalk::PushAfterWork(uv_work_t *req) {
  HandleScope scope;

  PushBaton *baton = static_cast<PushBaton *>(req->data);
  delete req;

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

Handle<Value> GitRevWalk::Next(const Arguments& args) {
  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  NextBaton* baton = new NextBaton();

  baton->request.data = baton;
  baton->revwalk = ObjectWrap::Unwrap<GitRevWalk>(args.This())->GetValue();
  baton->walkOver = false;
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, NextWork, NextAfterWork);

  return Undefined();
}

void GitRevWalk::NextWork(uv_work_t *req) {
  NextBaton *baton = static_cast<NextBaton *>(req->data);

  // baton->oid = NULL;
  int returnCode = git_revwalk_next(&baton->oid, baton->revwalk);
  if (returnCode != GIT_OK) {
    if (returnCode == GIT_REVWALKOVER) {
      baton->walkOver = true;
    } else {
      baton->error = giterr_last();
    }
  }
}

void GitRevWalk::NextAfterWork(uv_work_t *req) {
  HandleScope scope;

  NextBaton *baton = static_cast<NextBaton *>(req->data);
  delete req;

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

    Local<Object> oid = GitOid::constructor_template->NewInstance();
    GitOid *oidInstance = ObjectWrap::Unwrap<GitOid>(oid);
    oidInstance->SetValue(baton->oid);

    Local<Value> argv[3] = {
      Local<Value>::New(Null()),
      oid,
      Local<Value>::New(Boolean::New(baton->walkOver))
    };

    TryCatch try_catch;

    baton->callback->Call(Context::GetCurrent()->Global(), 3, argv);

    if(try_catch.HasCaught()) {
      FatalException(try_catch);
    }
  }
}

Handle<Value> GitRevWalk::Free(const Arguments& args) {
  HandleScope scope;

  GitRevWalk *revwalk = ObjectWrap::Unwrap<GitRevWalk>(args.This());

  revwalk->Free();

  return scope.Close( Undefined() );
}

Handle<Value> GitRevWalk::Repository(const Arguments& args) {
  HandleScope scope;

  GitRevWalk *revwalk = new GitRevWalk();

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repo is required and must be an Object.")));
  }

  GitRepo *repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject());
  repo->SetValue(revwalk->Repository());

  return scope.Close( Undefined() );
}
Persistent<FunctionTemplate> GitRevWalk::constructor_template;

/*
 * Copyright 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * @author Michael Robinson @codeofinterest <mike@pagesofinterest.net>
 *
 * Dual licensed under the MIT and GPL licenses.
 */

#include <v8.h>
#include <node.h>

#include "git2.h"

#include "../include/revwalk.h"
#include "../include/repo.h"
#include "../include/commit.h"
#include "../include/error.h"

#include "../include/functions/utilities.h"

using namespace v8;
using namespace node;

void GitRevWalk::Initialize(Handle<Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("RevWalk"));

  NODE_SET_PROTOTYPE_METHOD(tpl, "allocate", Allocate);
  NODE_SET_PROTOTYPE_METHOD(tpl, "reset", Reset);
  NODE_SET_PROTOTYPE_METHOD(tpl, "push", Push);
  NODE_SET_PROTOTYPE_METHOD(tpl, "next", Next);
  NODE_SET_PROTOTYPE_METHOD(tpl, "free", Free);

  // Local<Object> sort = Object::New();

  // sort->Set(String::New("NONE"), Integer::New(0));
  // sort->Set(String::New("TOPOLOGICAL"), Integer::New(1));
  // sort->Set(String::New("TIME"), Integer::New(2));
  // sort->Set(String::New("REVERSE"), Integer::New(4));

  // tpl->Set(String::New("sort"), sort);

  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("RevWalk"), constructor_template);
}

git_revwalk* GitRevWalk::GetValue() {
  return this->revwalk;
}
void GitRevWalk::SetValue(git_revwalk* revwalk) {
  this->revwalk = revwalk;
}
git_repository* GitRevWalk::GetRepo() {
  return this->repo;
}
void GitRevWalk::SetRepo(git_repository* repo) {
  this->repo = repo;
}

Handle<Value> GitRevWalk::Free(const Arguments& args) {
  HandleScope scope;

  GitRevWalk *revwalk = ObjectWrap::Unwrap<GitRevWalk>(args.This());

  git_revwalk_free(revwalk->revwalk);

  return Undefined();
}

Handle<Value> GitRevWalk::New(const Arguments& args) {
  HandleScope scope;

  GitRevWalk *revwalk = new GitRevWalk();
  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repo is required and must be an Object.")));
  }

  GitRepo *repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject());
  revwalk->SetRepo(repo->GetValue());

  revwalk->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> GitRevWalk::Reset(const Arguments& args) {
  HandleScope scope;

  GitRevWalk *revwalk = ObjectWrap::Unwrap<GitRevWalk>(args.This());
  git_revwalk_reset(revwalk->revwalk);

  return Undefined();
}

Handle<Value> GitRevWalk::Allocate(const Arguments& args) {
  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  AllocateBaton *baton = new AllocateBaton;
  baton->request.data = baton;
  baton->error = NULL;
  baton->revwalk = ObjectWrap::Unwrap<GitRevWalk>(args.This());
  baton->rawRevwalk = NULL;
  baton->rawRepo = baton->revwalk->GetRepo();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, AllocateWork, (uv_after_work_cb)AllocateAfterWork);

  return Undefined();
}
void GitRevWalk::AllocateWork(uv_work_t *req) {
  AllocateBaton *baton = static_cast<AllocateBaton *>(req->data);

  int returnCode = git_revwalk_new(&baton->rawRevwalk, baton->rawRepo);
  if (returnCode != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitRevWalk::AllocateAfterWork(uv_work_t *req) {
  HandleScope scope;
  AllocateBaton *baton = static_cast<AllocateBaton *>(req->data);


  if (success(baton->error, baton->callback)) {

    baton->revwalk->SetValue(baton->rawRevwalk);

    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      baton->revwalk->handle_
    };

    TryCatch try_catch;
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
    if (try_catch.HasCaught()) {
      node::FatalException(try_catch);
    }
  }
  delete req;
}

Handle<Value> GitRevWalk::Push(const Arguments& args) {
  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid is required and must be an Object.")));
  }

  if(args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  PushBaton* baton = new PushBaton;

  baton->request.data = baton;
  baton->error = NULL;
  baton->rawRevwalk = ObjectWrap::Unwrap<GitRevWalk>(args.This())->GetValue();
  baton->rawOid = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, PushWork, (uv_after_work_cb)PushAfterWork);

  return Undefined();
}
void GitRevWalk::PushWork(uv_work_t *req) {
  PushBaton *baton = static_cast<PushBaton *>(req->data);

  git_revwalk_sorting(baton->rawRevwalk, GIT_SORT_TIME | GIT_SORT_REVERSE);

  int returnCode = git_revwalk_push(baton->rawRevwalk, &baton->rawOid);
  if (returnCode) {
    baton->error = giterr_last();
  }
}
void GitRevWalk::PushAfterWork(uv_work_t *req) {
  HandleScope scope;
  PushBaton *baton = static_cast<PushBaton *>(req->data);

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
  delete req;
}

Handle<Value> GitRevWalk::Next(const Arguments& args) {
  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  NextBaton* baton = new NextBaton;

  baton->request.data = baton;
  baton->error = NULL;
  baton->rawRevwalk = ObjectWrap::Unwrap<GitRevWalk>(args.This())->GetValue();
  baton->walkOver = false;
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, NextWork, (uv_after_work_cb)NextAfterWork);

  return Undefined();
}
void GitRevWalk::NextWork(uv_work_t *req) {
  NextBaton *baton = static_cast<NextBaton *>(req->data);

  int returnCode = git_revwalk_next(&baton->rawOid, baton->rawRevwalk);
  if (returnCode != GIT_OK) {
    if (returnCode == GIT_ITEROVER) {
      baton->walkOver = true;
    } else {
      git_revwalk_reset(baton->rawRevwalk);
      baton->error = giterr_last();
    }
  }
}
void GitRevWalk::NextAfterWork(uv_work_t *req) {
  HandleScope scope;
  NextBaton *baton = static_cast<NextBaton *>(req->data);

  if (success(baton->error, baton->callback)) {
    git_oid *rawOid = (git_oid *)malloc(sizeof(git_oid));
    git_oid_cpy(rawOid, &baton->rawOid);
    Handle<Value> argv[1] = { External::New(rawOid) };
    Local<Object> oid = GitOid::constructor_template->NewInstance(1, argv);

    Local<Value> argv2[3] = {
      Local<Value>::New(Null()),
      oid,
      Local<Value>::New(Boolean::New(baton->walkOver))
    };

    TryCatch try_catch;
    baton->callback->Call(Context::GetCurrent()->Global(), 3, argv2);
    if(try_catch.HasCaught()) {
      FatalException(try_catch);
    }
  }
  delete req;
}

Persistent<Function> GitRevWalk::constructor_template;

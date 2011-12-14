/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#include <v8.h>
#include <node.h>

#include "../vendor/libgit2/include/git2.h"

#include "../include/revwalk.h"
#include "../include/repo.h"
#include "../include/commit.h"

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

int GitRevWalk::Push(git_oid* oid) {
  // Test
  git_revwalk_sorting(this->revwalk, GIT_SORT_TIME | GIT_SORT_REVERSE);

  return git_revwalk_push(this->revwalk, oid);
}

// Not for 0.0.1
//int GitRevWalk::Hide() {
//  git_revwalk_hide(this->revwalk);
//}

int GitRevWalk::Next(git_oid *oid) {
  return git_revwalk_next(oid, this->revwalk);
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

  GitRevWalk *revwalk = ObjectWrap::Unwrap<GitRevWalk>(args.This());
  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid is required and must be an Object.")));
  }

  GitOid *oid = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject());
  
  git_oid tmp = oid->GetValue();
  int err = revwalk->Push(&tmp);

  return scope.Close( Integer::New(err) );
}

Handle<Value> GitRevWalk::Next(const Arguments& args) {
  HandleScope scope;

  GitRevWalk* revwalk = ObjectWrap::Unwrap<GitRevWalk>(args.This());
  Local<Function> callback;

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid is required and must be an Object.")));
  }

  if(args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  callback = Local<Function>::Cast(args[1]);

  next_request* ar = new next_request();
  ar->revwalk = revwalk;
  ar->oid = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject());
  ar->callback = Persistent<Function>::New(callback);

  revwalk->Ref();

  eio_custom(EIO_Next, EIO_PRI_DEFAULT, EIO_AfterNext, ar);
  ev_ref(EV_DEFAULT_UC);

  return scope.Close( Undefined() );
}

void GitRevWalk::EIO_Next(eio_req *req) {
  next_request *ar = static_cast<next_request *>(req->data);
  git_oid oid = ar->oid->GetValue();

  ar->err = ar->revwalk->Next(&oid);
  ar->oid->SetValue(oid);

}

int GitRevWalk::EIO_AfterNext(eio_req *req) {
  HandleScope scope;

  next_request *ar = static_cast<next_request *>(req->data);
  ev_unref(EV_DEFAULT_UC);
  ar->revwalk->Unref();

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

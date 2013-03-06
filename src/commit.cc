/*
 * Copyright 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * Dual licensed under the MIT and GPL licenses.
 */

#include <string.h>
#include <v8.h>
#include <node.h>

#include "../vendor/libgit2/include/git2.h"
#include "cvv8/v8-convert.hpp"

#include "../include/reference.h"
#include "../include/sig.h"
#include "../include/repo.h"
#include "../include/oid.h"
#include "../include/tree.h"
#include "../include/commit.h"

using namespace v8;
using namespace node;

void GitCommit::Initialize(Handle<Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Commit"));

  NODE_SET_PROTOTYPE_METHOD(tpl, "lookup", Lookup);
  NODE_SET_PROTOTYPE_METHOD(tpl, "close", Close);
  NODE_SET_PROTOTYPE_METHOD(tpl, "id", Id);
  NODE_SET_PROTOTYPE_METHOD(tpl, "messageShort", MessageShort);
  NODE_SET_PROTOTYPE_METHOD(tpl, "message", Message);
  NODE_SET_PROTOTYPE_METHOD(tpl, "time", Time);
  NODE_SET_PROTOTYPE_METHOD(tpl, "timeOffset", TimeOffset);
  NODE_SET_PROTOTYPE_METHOD(tpl, "author", Author);
  NODE_SET_PROTOTYPE_METHOD(tpl, "tree", Tree);
  NODE_SET_PROTOTYPE_METHOD(tpl, "parentCount", ParentCount);
  NODE_SET_PROTOTYPE_METHOD(tpl, "parent", Parent);
  NODE_SET_PROTOTYPE_METHOD(tpl, "parentSync", ParentSync);

  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Commit"), constructor_template);
}

git_commit* GitCommit::GetValue() {
  return this->commit;
}

void GitCommit::SetValue(git_commit* commit) {
  this->commit = commit;
}

int GitCommit::Lookup(git_repository* repo, git_oid* oid) {
  int err = git_commit_lookup(&this->commit, repo, oid);

  return err;
}

void GitCommit::Close() {
  git_commit_close(this->commit);
}

const git_oid* GitCommit::Id() {
  return git_commit_id(this->commit);
}

const char* GitCommit::MessageShort() {
  return "";
  //return git_commit_message_short(this->commit);
}

const char* GitCommit::Message() {
  return git_commit_message(this->commit);
}

time_t GitCommit::Time() {
  return git_commit_time(this->commit);
}

int GitCommit::TimeOffset() {
  return git_commit_time_offset(this->commit);
}

const git_signature* GitCommit::Committer() {
  return git_commit_author(this->commit);
}

const git_signature* GitCommit::Author() {
  return git_commit_author(this->commit);
}

int GitCommit::Tree(git_tree** tree) {
  return git_commit_tree(tree, this->commit);
}

unsigned int GitCommit::ParentCount() {
  return git_commit_parentcount(this->commit);
}

int GitCommit::Parent(git_commit** commit, int pos) {
  return git_commit_parent(commit, this->commit, pos);
}

Handle<Value> GitCommit::New(const Arguments& args) {
  HandleScope scope;

  GitCommit *commit = new GitCommit();

  commit->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> GitCommit::NewInstance() {
  HandleScope scope;

  Local<Object> instance = constructor_template->NewInstance();

  return scope.Close(instance);
}

Handle<Value> GitCommit::Lookup(const Arguments& args) {
  HandleScope scope;

  GitCommit *commit = ObjectWrap::Unwrap<GitCommit>(args.This());
  Local<Function> callback;

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repo is required and must be an Object.")));
  }

  if(args.Length() == 1 || !args[1]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid is required and must be an Object.")));
  }

  if(args.Length() == 2 || !args[2]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  callback = Local<Function>::Cast(args[2]);

  lookup_request *ar = new lookup_request();
  ar->commit = commit;
  ar->repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject());
  ar->oid = ObjectWrap::Unwrap<GitOid>(args[1]->ToObject());
  ar->callback = Persistent<Function>::New(callback);

  commit->Ref();

  uv_work_t *req = new uv_work_t;
  req->data = ar;
  uv_queue_work(uv_default_loop(), req, LookupWork, LookupAfterWork);

  return scope.Close( Undefined() );
}

void GitCommit::LookupWork(uv_work_t *req) {
  lookup_request *ar = static_cast<lookup_request *>(req->data);

  git_oid oid = ar->oid->GetValue();
  ar->err = ar->commit->Lookup(ar->repo->GetValue(), &oid);
}

void GitCommit::LookupAfterWork(uv_work_t *req) {
  HandleScope scope;

  lookup_request *ar = static_cast<lookup_request *>(req->data);
  delete req;

  ar->commit->Unref();

  Handle<Value> argv[1];
  argv[0] = Integer::New(ar->err);

  TryCatch try_catch;

  ar->callback->Call(Context::GetCurrent()->Global(), 1, argv);

  if(try_catch.HasCaught()) {
    FatalException(try_catch);
  }

  ar->callback.Dispose();

  delete ar;
}

Handle<Value> GitCommit::Close(const Arguments& args) {
  HandleScope scope;

  GitCommit *commit = ObjectWrap::Unwrap<GitCommit>(args.This());
  commit->Close();

  return scope.Close( Undefined() );
}

Handle<Value> GitCommit::Id(const Arguments& args) {
  HandleScope scope;

  GitCommit *commit = ObjectWrap::Unwrap<GitCommit>(args.This());

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid is required and must be an Object.")));
  }

  GitOid *oid = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject());

  oid->SetValue(*const_cast<git_oid *>(commit->Id()));

  return scope.Close( Undefined() );
}

Handle<Value> GitCommit::MessageShort(const Arguments& args) {
  HandleScope scope;

  GitCommit *commit = ObjectWrap::Unwrap<GitCommit>(args.This());

  return scope.Close( String::New(commit->MessageShort()) );
}

Handle<Value> GitCommit::Message(const Arguments& args) {
  HandleScope scope;

  GitCommit *commit = ObjectWrap::Unwrap<GitCommit>(args.This());

  return scope.Close(String::New(commit->Message()));
}

Handle<Value> GitCommit::Time(const Arguments& args) {
  HandleScope scope;

  GitCommit *commit = ObjectWrap::Unwrap<GitCommit>(args.This());

  return scope.Close( Integer::New(commit->Time()) );
}

Handle<Value> GitCommit::TimeOffset(const Arguments& args) {
  HandleScope scope;

  GitCommit *commit = ObjectWrap::Unwrap<GitCommit>(args.This());

  return scope.Close( Integer::New(commit->TimeOffset()) );
}

Handle<Value> GitCommit::Committer(const Arguments& args) {
  HandleScope scope;

  GitCommit *commit = ObjectWrap::Unwrap<GitCommit>(args.This());

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Signature is required and must be an Object.")));
  }

  GitSig *sig = ObjectWrap::Unwrap<GitSig>(args[0]->ToObject());

  sig->SetValue(const_cast<git_signature *>(commit->Committer()));

  return scope.Close( Undefined() );
}

Handle<Value> GitCommit::Author(const Arguments& args) {
  HandleScope scope;

  GitCommit *commit = ObjectWrap::Unwrap<GitCommit>(args.This());

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Signature is required and must be an Object.")));
  }

  GitSig *sig = ObjectWrap::Unwrap<GitSig>(args[0]->ToObject());

  sig->SetValue(const_cast<git_signature *>(commit->Author()));

  return scope.Close( Undefined() );
}

Handle<Value> GitCommit::Tree(const Arguments& args) {
  HandleScope scope;

  GitCommit *commit = ObjectWrap::Unwrap<GitCommit>(args.This());

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Tree is required and must be an Object.")));
  }

  GitTree* g_tree = ObjectWrap::Unwrap<GitTree>(args[0]->ToObject());

  git_tree* tree;
  int err = commit->Tree(&tree);
  g_tree->SetValue(tree);

  return scope.Close( Integer::New(err) );
}

Handle<Value> GitCommit::ParentCount(const Arguments& args) {
  HandleScope scope;

  GitCommit *commit = ObjectWrap::Unwrap<GitCommit>(args.This());

  unsigned int count = commit->ParentCount();

  return scope.Close( Integer::New(count) );
}

Handle<Value> GitCommit::ParentSync(const Arguments& args) {
  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsNumber()) {
    return ThrowException(Exception::Error(String::New("Position must be a Number.")));
  }

  GitCommit *commit = ObjectWrap::Unwrap<GitCommit>(args.This());

  git_commit *parentCommitValue ;
  int errorCode = git_commit_parent(&parentCommitValue, commit->commit, args[0]->ToInteger()->Value());

  if (errorCode) {
    return ThrowException(Exception::Error(String::New(git_lasterror())));
  }

  Local<Object> parent = GitCommit::constructor_template->NewInstance();
  GitCommit *parentCommit = ObjectWrap::Unwrap<GitCommit>(parent);
  parentCommit->SetValue(parentCommitValue);

  return scope.Close(parent);
}

Handle<Value> GitCommit::Parent(const Arguments& args) {
  HandleScope scope;

  if(args.Length() != 2) {
    return ThrowException(Exception::Error(String::New("Position and callback are required")));
  }

  if(!args[0]->IsNumber()) {
    return ThrowException(Exception::Error(String::New("Position is required and must be a Number.")));
  }

  if(!args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  Local<Function> callback = Local<Function>::Cast(args[1]);

  ParentBaton* baton = new ParentBaton();
  baton->request.data = baton;
  baton->commit =  ObjectWrap::Unwrap<GitCommit>(args.This());
  baton->commit->Ref();
  baton->index = args[0]->ToInteger()->Value();
  baton->callback = Persistent<Function>::New(callback);

  uv_queue_work(uv_default_loop(), &baton->request, ParentWork, ParentAfterWork);

 return Undefined();
}

void GitCommit::ParentWork(uv_work_t* req) {
  ParentBaton* baton = static_cast<ParentBaton*>(req->data);

  baton->rawParentCommit = NULL;
  baton->errorCode = git_commit_parent(&baton->rawParentCommit, baton->commit->commit, baton->index);

  if (baton->errorCode) {
    baton->errorMessage = git_lasterror();
  }
}

void GitCommit::ParentAfterWork(uv_work_t* req) {
  HandleScope scope;

  ParentBaton* baton = static_cast<ParentBaton* >(req->data);
  delete req;

  if (baton->errorCode) {
    Local<Value> argv[1] = {
      Exception::Error(String::New(baton->errorMessage))
    };

    TryCatch try_catch;

    baton->callback->Call(Context::GetCurrent()->Global(), 1, argv);

    if (try_catch.HasCaught()) {
        node::FatalException(try_catch);
    }
  } else {

    Local<Object> parent = GitCommit::constructor_template->NewInstance();
    GitCommit *parentCommit = ObjectWrap::Unwrap<GitCommit>(parent);
    parentCommit->SetValue(baton->rawParentCommit);

    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      parent
    };

    TryCatch try_catch;
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
    if (try_catch.HasCaught()) {
        node::FatalException(try_catch);
    }
  }
  baton->commit->Unref();
  baton->callback.Dispose();
  delete baton;
}

Persistent<Function> GitCommit::constructor_template;

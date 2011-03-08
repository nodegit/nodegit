/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include "../vendor/libgit2/src/git2.h"

#include "reference.h"
#include "sig.h"
#include "repo.h"
#include "oid.h"
#include "tree.h"
#include "commit.h"

using namespace v8;
using namespace node;

void Commit::Initialize(Handle<Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("Commit"));

  NODE_SET_PROTOTYPE_METHOD(constructor_template, "lookup", Lookup);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "id", Id);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "messageShort", MessageShort);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "message", Message);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "time", Time);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "timeOffset", TimeOffset);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "author", Author);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "tree", Tree);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "parentCount", ParentCount);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "parent", Parent);

  target->Set(String::NewSymbol("Commit"), constructor_template->GetFunction());
}

git_commit* Commit::GetValue() {
  return this->commit;
}

void Commit::SetValue(git_commit* commit) {
  this->commit = commit;
}

int Commit::Lookup(git_repository* repo, git_oid* oid) {
  return git_commit_lookup(&this->commit, repo, oid);
}

int Commit::New(git_repository* repo) {
  return git_commit_new(&this->commit, repo);
}

const git_oid* Commit::Id() {
  return git_commit_id(this->commit);
}

const char* Commit::MessageShort() {
  return git_commit_message_short(this->commit);
}

const char* Commit::Message() {
  return git_commit_message(this->commit);
}

time_t Commit::Time() {
  return git_commit_time(this->commit);
}

int Commit::TimeOffset() {
  return git_commit_time_offset(this->commit);
}

const git_signature* Commit::Committer() {
  return git_commit_author(this->commit);
}

const git_signature* Commit::Author() {
  return git_commit_author(this->commit);
}

const git_tree* Commit::Tree() {
  return git_commit_tree(this->commit);
}

unsigned int Commit::ParentCount() {
  return git_commit_parentcount(this->commit);
}

git_commit* Commit::Parent(int pos) {
  return git_commit_parent(this->commit, pos);
}

Handle<Value> Commit::New(const Arguments& args) {
  HandleScope scope;

  Commit *commit = new Commit();

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repo is required and must be an Object.")));
  }

  Repo *repo = ObjectWrap::Unwrap<Repo>(args[0]->ToObject());
  commit->New((git_repository *)repo);

  commit->Wrap(args.This());

  return args.This();
}

Handle<Value> Commit::Lookup(const Arguments& args) {
  Commit *commit = ObjectWrap::Unwrap<Commit>(args.This());
  Local<Function> callback;

  HandleScope scope;

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
  ar->repo = ObjectWrap::Unwrap<Repo>(args[0]->ToObject());
  ar->oid = ObjectWrap::Unwrap<Oid>(args[1]->ToObject());
  ar->callback = Persistent<Function>::New(callback);

  commit->Ref();

  eio_custom(EIO_Lookup, EIO_PRI_DEFAULT, EIO_AfterLookup, ar);
  ev_ref(EV_DEFAULT_UC);

  return Undefined();
}

int Commit::EIO_Lookup(eio_req *req) {
  lookup_request *ar = static_cast<lookup_request *>(req->data);

  ar->err = Persistent<Value>::New(Integer::New(ar->commit->Lookup(ar->repo->GetValue(), ar->oid->GetValue())));

  return 0;
}

int Commit::EIO_AfterLookup(eio_req *req) {
  HandleScope scope;

  lookup_request *ar = static_cast<lookup_request *>(req->data);
  ev_unref(EV_DEFAULT_UC);
  ar->commit->Unref();

  git_commit *commit = ar->commit->GetValue();

  Local<Value> argv[1];
  argv[0] = *ar->err;

  TryCatch try_catch;

  ar->callback->Call(Context::GetCurrent()->Global(), 1, argv);

  if(try_catch.HasCaught())
    FatalException(try_catch);
    
  ar->err.Dispose();
  ar->callback.Dispose();

  delete ar;

  return 0;
}

Handle<Value> Commit::Id(const Arguments& args) {
  Commit *commit = ObjectWrap::Unwrap<Commit>(args.This());

  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid is required and must be an Object.")));
  }

  Oid *oid = ObjectWrap::Unwrap<Oid>(args[0]->ToObject());

  oid->SetValue(const_cast<git_oid *>(commit->Id()));
  
  return Undefined();
}

Handle<Value> Commit::MessageShort(const Arguments& args) {
  Commit *commit = ObjectWrap::Unwrap<Commit>(args.This());

  HandleScope scope;
  
  return String::New(commit->MessageShort());
}

Handle<Value> Commit::Message(const Arguments& args) {
  Commit *commit = ObjectWrap::Unwrap<Commit>(args.This());

  HandleScope scope;
  
  return String::New(commit->Message());
}

Handle<Value> Commit::Time(const Arguments& args) {
  Commit *commit = ObjectWrap::Unwrap<Commit>(args.This());

  HandleScope scope;
  
  return Integer::New(commit->Time());
}

Handle<Value> Commit::TimeOffset(const Arguments& args) {
  Commit *commit = ObjectWrap::Unwrap<Commit>(args.This());

  HandleScope scope;
  
  return Integer::New(commit->TimeOffset());
}

Handle<Value> Commit::Committer(const Arguments& args) {
  Commit *commit = ObjectWrap::Unwrap<Commit>(args.This());

  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Signature is required and must be an Object.")));
  }

  Sig *sig = ObjectWrap::Unwrap<Sig>(args[0]->ToObject());

  sig->SetValue(const_cast<git_signature *>(commit->Committer()));
  
  return Undefined();
}

Handle<Value> Commit::Author(const Arguments& args) {
  Commit *commit = ObjectWrap::Unwrap<Commit>(args.This());

  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Signature is required and must be an Object.")));
  }

  Sig *sig = ObjectWrap::Unwrap<Sig>(args[0]->ToObject());

  sig->SetValue(const_cast<git_signature *>(commit->Author()));
  
  return Undefined();
}

Handle<Value> Commit::Tree(const Arguments& args) {
  Commit *commit = ObjectWrap::Unwrap<Commit>(args.This());

  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Tree is required and must be an Object.")));
  }

  GitTree* tree = ObjectWrap::Unwrap<GitTree>(args[0]->ToObject());

  tree->SetValue(const_cast<git_tree *>(commit->Tree()));

  return Undefined();
}
//Handle<Value> Commit::Tree(const Arguments& args) {
//  Commit *commit = ObjectWrap::Unwrap<Commit>(args.This());
//  Local<Function> callback;
//
//  HandleScope scope;
//
//  if(args.Length() == 0 || !args[0]->IsObject()) {
//    return ThrowException(Exception::Error(String::New("Tree is required and must be an Object.")));
//  }
//
//  callback = Local<Function>::Cast(args[1]);
//
//  tree_request *ar = new tree_request();
//  ar->commit = commit;
//  ar->repo = ObjectWrap::Unwrap<Tree>(args[0]->ToObject());
//  ar->callback = Persistent<Function>::New(callback);
//
//  commit->Ref();
//
//  eio_custom(EIO_Tree, EIO_PRI_DEFAULT, EIO_AfterTree, ar);
//  ev_ref(EV_DEFAULT_UC);
//
//  return Undefined();
//}
//
//int Commit::EIO_Tree(eio_req *req) {
//  tree_request *ar = static_cast<tree_request *>(req->data);
//
//  git_tree *tree = ar->commit->Tree();
//
//  ar->tree->SetValue(tree);
//
//  return 0;
//}
//
//int Commit::EIO_AfterTree(eio_req *req) {
//  HandleScope scope;
//
//  tree_request *ar = static_cast<tree_request *>(req->data);
//  ev_unref(EV_DEFAULT_UC);
//  ar->commit->Unref();
//
//  Local<Value> argv[1];
//
//  TryCatch try_catch;
//
//  ar->callback->Call(Context::GetCurrent()->Global(), 1, argv);
//
//  if(try_catch.HasCaught())
//    FatalException(try_catch);
//    
//  ar->err.Dispose();
//  ar->callback.Dispose();
//
//  delete ar;
//
//  return 0;
//}

Handle<Value> Commit::ParentCount(const Arguments& args) {
  Commit *commit = ObjectWrap::Unwrap<Commit>(args.This());

  HandleScope scope;

  unsigned int count = commit->ParentCount();

  return Integer::New(count);
}

Handle<Value> Commit::Parent(const Arguments& args) {
  Commit *commit = ObjectWrap::Unwrap<Commit>(args.This());

  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Commit is required and must be an Object.")));
  }

  if(args.Length() == 1 || !args[1]->IsNumber()) {
    return ThrowException(Exception::Error(String::New("Position is required and must be a Number.")));
  }

  Commit* out = ObjectWrap::Unwrap<Commit>(args[0]->ToObject());
  int index = args[1]->ToInteger()->Value();

  out->SetValue(commit->Parent(index));

  return Undefined();
}
Persistent<FunctionTemplate> Commit::constructor_template;

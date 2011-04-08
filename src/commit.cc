/*
 * Copyright 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * Dual licensed under the MIT and GPL licenses.
 */

#include <string.h>
#include <v8.h>
#include <node.h>
#include <node_events.h>

#include "../vendor/libgit2/include/git2.h"

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

git_commit* GitCommit::GetValue() {
  return this->commit;
}

void GitCommit::SetValue(git_commit* commit) {
  this->commit = commit;
}

int GitCommit::Lookup(git_oid* oid) {
  git_commit* commit;

  //this->oid = oid;

  //int err = git_commit_lookup(&commit, this->repo, oid);

  //this->commit = commit;

  //return err;
  return 0;
}

const git_oid* GitCommit::Id() {
  return git_commit_id(this->commit);
}

const char* GitCommit::MessageShort() {
  return git_commit_message_short(this->commit);
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

  return args.This();
}

Handle<Value> GitCommit::Lookup(const Arguments& args) {
  GitCommit *commit = ObjectWrap::Unwrap<GitCommit>(args.This());
  Local<Function> callback;

  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid is required and must be an Object.")));
  }

  if(args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  callback = Local<Function>::Cast(args[1]);

  lookup_request *ar = new lookup_request();
  ar->commit = commit;
  ar->oid = ObjectWrap::Unwrap<Oid>(args[0]->ToObject());
  ar->callback = Persistent<Function>::New(callback);

  commit->Ref();

  eio_custom(EIO_Lookup, EIO_PRI_DEFAULT, EIO_AfterLookup, ar);
  ev_ref(EV_DEFAULT_UC);

  return Undefined();
}

int GitCommit::EIO_Lookup(eio_req *req) {
  lookup_request *ar = static_cast<lookup_request *>(req->data);

  ar->err = ar->commit->Lookup(ar->oid->GetValue());

  return 0;
}

int GitCommit::EIO_AfterLookup(eio_req *req) {
  HandleScope scope;

  lookup_request *ar = static_cast<lookup_request *>(req->data);
  ev_unref(EV_DEFAULT_UC);
  ar->commit->Unref();

  Local<Value> argv[0];
  argv[0] = Integer::New(ar->err);

  TryCatch try_catch;

  ar->callback->Call(Context::GetCurrent()->Global(), 1, argv);

  if(try_catch.HasCaught())
    FatalException(try_catch);
    
  ar->callback.Dispose();

  delete ar;

  return 0;
}

Handle<Value> GitCommit::Id(const Arguments& args) {
  GitCommit *commit = ObjectWrap::Unwrap<GitCommit>(args.This());

  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid is required and must be an Object.")));
  }

  Oid *oid = ObjectWrap::Unwrap<Oid>(args[0]->ToObject());

  oid->SetValue(const_cast<git_oid *>(commit->Id()));
  
  return Undefined();
}

Handle<Value> GitCommit::MessageShort(const Arguments& args) {
  GitCommit *commit = ObjectWrap::Unwrap<GitCommit>(args.This());

  HandleScope scope;
  
  return String::New(commit->MessageShort());
}

Handle<Value> GitCommit::Message(const Arguments& args) {
  GitCommit *commit = ObjectWrap::Unwrap<GitCommit>(args.This());

  HandleScope scope;
  
  return String::New(commit->Message());
}

Handle<Value> GitCommit::Time(const Arguments& args) {
  GitCommit *commit = ObjectWrap::Unwrap<GitCommit>(args.This());

  HandleScope scope;
  
  return Integer::New(commit->Time());
}

Handle<Value> GitCommit::TimeOffset(const Arguments& args) {
  GitCommit *commit = ObjectWrap::Unwrap<GitCommit>(args.This());

  HandleScope scope;
  
  return Integer::New(commit->TimeOffset());
}

Handle<Value> GitCommit::Committer(const Arguments& args) {
  GitCommit *commit = ObjectWrap::Unwrap<GitCommit>(args.This());

  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Signature is required and must be an Object.")));
  }

  Sig *sig = ObjectWrap::Unwrap<Sig>(args[0]->ToObject());

  sig->SetValue(const_cast<git_signature *>(commit->Committer()));
  
  return Undefined();
}

Handle<Value> GitCommit::Author(const Arguments& args) {
  GitCommit *commit = ObjectWrap::Unwrap<GitCommit>(args.This());

  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Signature is required and must be an Object.")));
  }

  Sig *sig = ObjectWrap::Unwrap<Sig>(args[0]->ToObject());

  sig->SetValue(const_cast<git_signature *>(commit->Author()));
  
  return Undefined();
}

Handle<Value> GitCommit::Tree(const Arguments& args) {
  GitCommit *commit = ObjectWrap::Unwrap<GitCommit>(args.This());

  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Tree is required and must be an Object.")));
  }

  git_tree* in;
  GitTree* tree = ObjectWrap::Unwrap<GitTree>(args[0]->ToObject());

  int err = commit->Tree(&in);
  tree->SetValue(in);

  return Integer::New(err);
}
//Handle<Value> GitCommit::Tree(const Arguments& args) {
//  GitCommit *commit = ObjectWrap::Unwrap<GitCommit>(args.This());
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
//int GitCommit::EIO_Tree(eio_req *req) {
//  tree_request *ar = static_cast<tree_request *>(req->data);
//
//  git_tree *tree = ar->commit->Tree();
//
//  ar->tree->SetValue(tree);
//
//  return 0;
//}
//
//int GitCommit::EIO_AfterTree(eio_req *req) {
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

Handle<Value> GitCommit::ParentCount(const Arguments& args) {
  GitCommit *commit = ObjectWrap::Unwrap<GitCommit>(args.This());

  HandleScope scope;

  unsigned int count = commit->ParentCount();

  return Integer::New(count);
}

Handle<Value> GitCommit::Parent(const Arguments& args) {
  GitCommit *commit = ObjectWrap::Unwrap<GitCommit>(args.This());

  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Commit is required and must be an Object.")));
  }

  if(args.Length() == 1 || !args[1]->IsNumber()) {
    return ThrowException(Exception::Error(String::New("Position is required and must be a Number.")));
  }

  GitCommit* out = ObjectWrap::Unwrap<GitCommit>(args[0]->ToObject());
  git_commit* in;
  int index = args[1]->ToInteger()->Value();

  int err = commit->Parent(&in, index);
  out->SetValue(in);

  return Integer::New(err);
}

Persistent<FunctionTemplate> GitCommit::constructor_template;

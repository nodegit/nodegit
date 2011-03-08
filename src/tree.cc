/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include "../vendor/libgit2/src/git2.h"

#include "repo.h"
#include "tree.h"

using namespace v8;
using namespace node;

void GitTree::Initialize (Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("Tree"));

  NODE_SET_PROTOTYPE_METHOD(constructor_template, "entryCount", EntryCount);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "entryByIndex", EntryByIndex);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "sortEntries", EntryCount);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "clearEntries", ClearEntries);

  target->Set(String::NewSymbol("Tree"), constructor_template->GetFunction());
}

git_tree* GitTree::GetValue() {
  return this->tree;
}

void GitTree::SetValue(git_tree* tree) {
  this->tree = tree;
}

int GitTree::New(git_repository* repo) {
  return git_tree_new(&this->tree, repo);
}

size_t GitTree::EntryCount() {
  return git_tree_entrycount(&*this->tree);
}

int GitTree::SortEntries() {
  //return git_tree_sort_entries(this->tree);
  return 0;
}

void GitTree::ClearEntries() {
  git_tree_clear_entries(this->tree);
}

Handle<Value> GitTree::New(const Arguments& args) {
  HandleScope scope;

  GitTree *tree = new GitTree();

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repo is required and must be an Object.")));
  }

  Repo *repo = ObjectWrap::Unwrap<Repo>(args[0]->ToObject());
  int err = tree->New((git_repository *)repo);
    
  tree->Wrap(args.This());

  args.This()->Set(String::New("error"), Integer::New(err));

  return args.This();
}

Handle<Value> GitTree::EntryCount(const Arguments& args) {
  HandleScope scope;

  GitTree *tree = ObjectWrap::Unwrap<GitTree>(args.This());

  int count = tree->EntryCount();
    
  return Local<Value>::New(Integer::New(count));
}

Handle<Value> GitTree::EntryByIndex(const Arguments& args) {
  HandleScope scope;

  GitTree *tree = ObjectWrap::Unwrap<GitTree>(args.This());

    
  return Undefined();
  //return Local<Value>::New(Integer::New(count));
}

Handle<Value> GitTree::SortEntries(const Arguments& args) {
  HandleScope scope;

  GitTree *tree = ObjectWrap::Unwrap<GitTree>(args.This());

  int err = tree->SortEntries();
  
  return Integer::New(err);
}

Handle<Value> GitTree::ClearEntries(const Arguments& args) {
  HandleScope scope;

  GitTree *tree = ObjectWrap::Unwrap<GitTree>(args.This());

  tree->ClearEntries();
    
  return Undefined();
}
Persistent<FunctionTemplate> GitTree::constructor_template;

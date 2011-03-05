/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include <git2.h>

#include "repo.h"
#include "tree.h"

using namespace v8;
using namespace node;

void Tree::Initialize (Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("Tree"));

  target->Set(String::NewSymbol("Tree"), constructor_template->GetFunction());
}

git_tree* Tree::GetValue() {
  return this->tree;
}

void Tree::SetValue(git_tree* tree) {
  this->tree = tree;
}

int Tree::New(git_repository* repo) {
  return git_tree_new(&this->tree, repo);
}

Handle<Value> Tree::New(const Arguments& args) {
  HandleScope scope;

  Tree *tree = new Tree();

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repo is required and must be an Object.")));
  }

  Repo *repo = ObjectWrap::Unwrap<Repo>(args[0]->ToObject());
  int err = tree->New((git_repository *)repo);
    
  tree->Wrap(args.This());

  return args.This();
}
Persistent<FunctionTemplate> Tree::constructor_template;

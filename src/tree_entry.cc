/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include "../vendor/libgit2/src/git2.h"

#include "repo.h"
#include "tree.h"
#include "tree_entry.h"

using namespace v8;
using namespace node;

void GitTreeEntry::Initialize(Handle<v8::Object> target) {
  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("TreeEntry"));

  NODE_SET_PROTOTYPE_METHOD(constructor_template, "name", Name);

  target->Set(String::NewSymbol("TreeEntry"), constructor_template->GetFunction());
}

void GitTreeEntry::SetValue(git_tree_entry* entry) {
  this->entry = entry;
}

const char* GitTreeEntry::Name() {
  return git_tree_entry_name(this->entry);
}

int GitTreeEntry::ToObject(git_object** object, git_tree_entry* entry) {
  return git_tree_entry_2object(object, entry);
}

Handle<Value> GitTreeEntry::New(const Arguments& args) {
  HandleScope scope;

  GitTreeEntry *entry = new GitTreeEntry();

  entry->Wrap(args.This());

  return args.This();
}

Handle<Value> GitTreeEntry::Name(const Arguments& args) {
  HandleScope scope;

  GitTreeEntry *entry = ObjectWrap::Unwrap<GitTreeEntry>(args.This());

  return String::New(entry->Name());
}
Persistent<FunctionTemplate> GitTreeEntry::constructor_template;

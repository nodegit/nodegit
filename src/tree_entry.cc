/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include "../vendor/libgit2/src/git2.h"

#include "repo.h"
#include "tree.h"
#include "git_tree.h"

using namespace v8;
using namespace node;

void GitTreeEntry::Initialize (Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("TreeEntry"));

  NODE_SET_PROTOTYPE_METHOD(constructor_template, "entryCount", EntryCount);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "entryByIndex", EntryByIndex);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "sortEntries", EntryCount);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "clearEntries", ClearEntries);

  target->Set(String::NewSymbol("TreeEntry"), constructor_template->GetFunction());
}

void SetValue(git_tree_entry* entry) {
  this->entry = entry;
}

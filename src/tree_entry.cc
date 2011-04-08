/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include "../vendor/libgit2/include/git2.h"

#include "../include/repo.h"
#include "../include/blob.h"
#include "../include/tree.h"
#include "../include/object.h"
#include "../include/oid.h"
#include "../include/tree_entry.h"

using namespace v8;
using namespace node;

void GitTreeEntry::Initialize(Handle<v8::Object> target) {
  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("TreeEntry"));

  NODE_SET_PROTOTYPE_METHOD(constructor_template, "name", Name);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "toObject", ToObject);

  target->Set(String::NewSymbol("TreeEntry"), constructor_template->GetFunction());
}

void GitTreeEntry::SetValue(git_tree_entry* entry) {
  this->entry = entry;
}

const char* GitTreeEntry::Name() {
  return git_tree_entry_name(this->entry);
}

const git_oid* GitTreeEntry::Id() {
  return git_tree_entry_id(this->entry);
}

int GitTreeEntry::ToObject(git_object** obj) {
  //TODO: Implement correct arguments
  //return git_tree_entry_2object(obj, this->entry);
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

Handle<Value> GitTreeEntry::Id(const Arguments& args) {
  HandleScope scope;

  GitTreeEntry *entry = ObjectWrap::Unwrap<GitTreeEntry>(args.This());

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid is required and must be an Object.")));
  }

  Oid* oid = ObjectWrap::Unwrap<Oid>(args[0]->ToObject());

  oid->SetValue(const_cast<git_oid *>(entry->Id()));
  
  return Undefined();
}

Handle<Value> GitTreeEntry::ToObject(const Arguments& args) {
  HandleScope scope;

  GitTreeEntry *entry = ObjectWrap::Unwrap<GitTreeEntry>(args.This());

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Blob is required and must be an Object.")));
  }

  GitBlob* blob = ObjectWrap::Unwrap<GitBlob>(args[0]->ToObject());

  git_object* out;
  entry->ToObject(&out);
  blob->SetValue((git_blob *)out);
  
  return Undefined();
}
Persistent<FunctionTemplate> GitTreeEntry::constructor_template;

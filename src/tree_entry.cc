/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#include <v8.h>
#include <node.h>

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
  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("TreeEntry"));

  NODE_SET_PROTOTYPE_METHOD(tpl, "toObject", ToObject);

  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("TreeEntry"), constructor_template);
}

git_tree_entry* GitTreeEntry::GetValue() {
  return this->entry;
}

void GitTreeEntry::SetValue(git_tree_entry* entry) {
  this->entry = entry;
}

Handle<Value> GitTreeEntry::New(const Arguments& args) {
  HandleScope scope;

  GitTreeEntry *entry = new GitTreeEntry();

  entry->Wrap(args.This());

  return scope.Close(args.This());
}


Handle<Value> GitTreeEntry::ToObject(const Arguments& args) {
  HandleScope scope;

  GitTreeEntry *entry = ObjectWrap::Unwrap<GitTreeEntry>(args.This());

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repo is required and must be an Object.")));
  }

  if(args.Length() == 1 || !args[1]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Object is required and must be an Object.")));
  }

  GitRepo* repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject());
  GitObject* object = ObjectWrap::Unwrap<GitObject>(args[1]->ToObject());

  git_object* out;
  entry->ToObject(repo->GetValue(), &out);

  object->SetValue(out);

  return scope.Close( Undefined() );
}
Persistent<Function> GitTreeEntry::constructor_template;


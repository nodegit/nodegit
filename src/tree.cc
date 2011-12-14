/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#include <v8.h>
#include <node.h>

#include "../vendor/libgit2/include/git2.h"

#include "../include/repo.h"
#include "../include/oid.h"
#include "../include/tree.h"
#include "../include/tree_entry.h"

using namespace v8;
using namespace node;

void GitTree::Initialize (Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("Tree"));

  NODE_SET_PROTOTYPE_METHOD(constructor_template, "lookup", Lookup);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "entryCount", EntryCount);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "entryByIndex", EntryByIndex);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "entryByName", EntryByName);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "sortEntries", EntryCount);

  target->Set(String::NewSymbol("Tree"), constructor_template->GetFunction());
}

git_tree* GitTree::GetValue() {
  return this->tree;
}

void GitTree::SetValue(git_tree* tree) {
  this->tree = tree;
}

int GitTree::Lookup(git_repository* repo, const git_oid* id) {
  return git_tree_lookup(&this->tree, repo, id);
}

size_t GitTree::EntryCount() {
  return git_tree_entrycount(this->tree);
}

git_tree_entry* GitTree::EntryByIndex(int idx) {
  return const_cast<git_tree_entry*>(git_tree_entry_byindex(this->tree, idx));
}

git_tree_entry* GitTree::EntryByName(const char* name) {
  return const_cast<git_tree_entry*>(git_tree_entry_byname(this->tree, name));
}

int GitTree::SortEntries() {
  //return git_tree_sort_entries(this->tree);
  return 0;
}

Handle<Value> GitTree::New(const Arguments& args) {
  HandleScope scope;

  GitTree *tree = new GitTree();

  tree->Wrap(args.This());

  return scope.Close( args.This() );
}

Handle<Value> GitTree::Lookup(const Arguments& args) {
  HandleScope scope;

  GitTree *tree = ObjectWrap::Unwrap<GitTree>(args.This());

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repo is required and must be an Object.")));
  }

  if(args.Length() == 1 || !args[1]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid is required and must be an Object.")));
  }

  GitRepo* repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject());
  GitOid* oid = ObjectWrap::Unwrap<GitOid>(args[1]->ToObject());

  git_oid ref_oid = oid->GetValue();
  int err = tree->Lookup(repo->GetValue(), &ref_oid);

  return scope.Close( Integer::New(err) );

//  if(args.Length() == 2 || !args[2]->IsFunction()) {
//    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
//  }
//
//  callback = Local<Function>::Cast(args[2]);
//
//  lookup_request *lr = new lookup_request();
//  lr->tree = tree;
//  lr->repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject());
//  lr->oid = ObjectWrap::Unwrap<GitOid>(args[1]->ToObject());
//  lr->callback = Persistent<Function>::New(callback);
//
//  tree->Ref();
//
//  eio_custom(EIO_Lookup, EIO_PRI_DEFAULT, EIO_AfterLookup, lr);
//  ev_ref(EV_DEFAULT_UC);
//
//  return scope.Close( Undefined() );
}

//int GitTree::EIO_Lookup(eio_req *req) {
//  lookup_request *lr = static_cast<lookup_request *>(req->data);
//
//  git_oid oid = lr->oid->GetValue();
//  lr->err = lr->tree->Lookup(lr->repo->GetValue(), &oid);
//
//  return 0;
//}

//int GitTree::EIO_AfterLookup(eio_req *req) {
//  lookup_request *lr = static_cast<lookup_request *>(req->data);
//
//  ev_unref(EV_DEFAULT_UC);
//  lr->tree->Unref();
//
//  Handle<Value> argv[1];
//  argv[0] = Integer::New(lr->err);
//
//  TryCatch try_catch;
//
//  lr->callback->Call(Context::GetCurrent()->Global(), 1, argv);
//
//  if(try_catch.HasCaught())
//    FatalException(try_catch);
//    
//  lr->callback.Dispose();
//
//  delete lr;
//
//  return 0;
//}

Handle<Value> GitTree::EntryCount(const Arguments& args) {
  HandleScope scope;

  GitTree *tree = ObjectWrap::Unwrap<GitTree>(args.This());

  int count = tree->EntryCount();
    
  return scope.Close( Integer::New(count) );
}

Handle<Value> GitTree::EntryByIndex(const Arguments& args) {
  HandleScope scope;

  GitTree *tree = ObjectWrap::Unwrap<GitTree>(args.This());
  Local<Function> callback;

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("TreeEntry is required and must be a Object.")));
  }

  if(args.Length() == 1 || !args[1]->IsNumber()) {
    return ThrowException(Exception::Error(String::New("Index is required and must be a Number.")));
  }

  if(args.Length() == 2 || !args[2]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  callback = Local<Function>::Cast(args[2]);

  entryindex_request *er = new entryindex_request();
  er->tree = tree;
  er->entry = ObjectWrap::Unwrap<GitTreeEntry>(args[0]->ToObject());
  er->idx = args[1]->ToInteger()->Value();
  er->callback = Persistent<Function>::New(callback);

  tree->Ref();

  eio_custom(EIO_EntryByIndex, EIO_PRI_DEFAULT, EIO_AfterEntryByIndex, er);
  ev_ref(EV_DEFAULT_UC);

  return scope.Close( Undefined() );
}

void GitTree::EIO_EntryByIndex(eio_req *req) {
  entryindex_request *er = static_cast<entryindex_request *>(req->data);

  er->entry->SetValue(er->tree->EntryByIndex(er->idx));

}

int GitTree::EIO_AfterEntryByIndex(eio_req *req) {
  entryindex_request *er = static_cast<entryindex_request *>(req->data);

  ev_unref(EV_DEFAULT_UC);
  er->tree->Unref();

  Handle<Value> argv[0];

  TryCatch try_catch;

  er->callback->Call(Context::GetCurrent()->Global(), 0, argv);

  if(try_catch.HasCaught())
    FatalException(try_catch);
    
  er->callback.Dispose();

  delete er;

  return 0;
}

Handle<Value> GitTree::EntryByName(const Arguments& args) {
  HandleScope scope;

  GitTree *tree = ObjectWrap::Unwrap<GitTree>(args.This());

  Local<Function> callback;

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("TreeEntry is required and must be a Object.")));
  }

  if(args.Length() == 1 || !args[1]->IsString()) {
    return ThrowException(Exception::Error(String::New("Name is required and must be a String.")));
  }

  if(args.Length() == 2 || !args[2]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  callback = Local<Function>::Cast(args[2]);
  String::Utf8Value name(args[1]->ToString());

  entryname_request *er = new entryname_request();
  er->tree = tree;
  er->entry = ObjectWrap::Unwrap<GitTreeEntry>(args[0]->ToObject());
  er->name = *name;
  er->callback = Persistent<Function>::New(callback);

  tree->Ref();

  eio_custom(EIO_EntryByName, EIO_PRI_DEFAULT, EIO_AfterEntryByName, er);
  ev_ref(EV_DEFAULT_UC);

  return scope.Close( Undefined() );
}

void GitTree::EIO_EntryByName(eio_req *req) {
  entryname_request *er = static_cast<entryname_request *>(req->data);

  er->entry->SetValue(er->tree->EntryByName(er->name.c_str()));

}

int GitTree::EIO_AfterEntryByName(eio_req *req) {
  entryname_request *er = static_cast<entryname_request *>(req->data);

  ev_unref(EV_DEFAULT_UC);
  er->tree->Unref();

  Handle<Value> argv[1];
  argv[0] = Boolean::New(er->entry->GetValue() != NULL);

  TryCatch try_catch;

  er->callback->Call(Context::GetCurrent()->Global(), 1, argv);

  if(try_catch.HasCaught())
    FatalException(try_catch);
    
  er->callback.Dispose();

  delete er;

  return 0;
}

Handle<Value> GitTree::SortEntries(const Arguments& args) {
  HandleScope scope;

  GitTree *tree = ObjectWrap::Unwrap<GitTree>(args.This());

  int err = tree->SortEntries();
  
  return scope.Close( Integer::New(err) );
}

Handle<Value> GitTree::ClearEntries(const Arguments& args) {
  HandleScope scope;

  //GitTree *tree = ObjectWrap::Unwrap<GitTree>(args.This());

  //tree->ClearEntries();

  return scope.Close( Undefined() );
}
Persistent<FunctionTemplate> GitTree::constructor_template;

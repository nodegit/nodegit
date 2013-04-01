/*
 * Copyright 2013, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * @author Michael Robinson @codeofinterest <mike@pagesofinterest.net>
 *
 * Dual licensed under the MIT and GPL licenses.
 */

#include <v8.h>
#include <node.h>

#include "../vendor/libgit2/include/git2.h"

#include "../include/repo.h"
#include "../include/oid.h"
#include "../include/tree.h"
#include "../include/tree_entry.h"
#include "../include/error.h"

#include "../include/functions/string.h"
#include "../include/functions/utilities.h"

using namespace v8;
using namespace node;

void GitTree::Initialize (Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Tree"));

  NODE_SET_PROTOTYPE_METHOD(tpl, "entryByPath", EntryByPath);

  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Tree"), constructor_template);
}

git_tree* GitTree::GetValue() {
  return this->tree;
}
void GitTree::SetValue(git_tree* tree) {
  this->tree = tree;
}

Handle<Value> GitTree::New(const Arguments& args) {
  HandleScope scope;

  GitTree *tree = new GitTree();

  tree->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> GitTree::EntryByPath(const Arguments& args) {
  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("Path is required and must be a String.")));
  }

  if(args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  EntryByPathBaton *baton = new EntryByPathBaton;
  baton->request.data = baton;
  baton->error = NULL;
  baton->rawEntry = NULL;

  GitTree *tree = ObjectWrap::Unwrap<GitTree>(args.This());
  tree->Ref();

  baton->rawTree = tree->GetValue();
  baton->path = stringArgToString(args[0]->ToString());
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, EntryByPathWork, (uv_after_work_cb)EntryByPathAfterWork);

  return Undefined();
}
void GitTree::EntryByPathWork(uv_work_t *req) {
  EntryByPathBaton *baton = static_cast<EntryByPathBaton *>(req->data);

  int returnCode = git_tree_entry_bypath(&baton->rawEntry, baton->rawTree, baton->path.c_str());
  if (returnCode != GIT_OK) {
    baton->error = giterr_last();
  }
}
void GitTree::EntryByPathAfterWork(uv_work_t *req) {
  HandleScope scope;
  EntryByPathBaton *baton = static_cast<EntryByPathBaton *>(req->data);

  if (success(baton->error, baton->callback)) {
    Local<Object> entry = GitTreeEntry::constructor_template->NewInstance();
    GitTreeEntry *entryInstance = ObjectWrap::Unwrap<GitTreeEntry>(entry);
    entryInstance->SetValue(baton->rawEntry);

    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      entry
    };

    TryCatch try_catch;
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
    if (try_catch.HasCaught()) {
      node::FatalException(try_catch);
    }
  }
  delete req;
}

Persistent<Function> GitTree::constructor_template;

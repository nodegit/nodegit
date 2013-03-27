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
#include "../include/error.h"

using namespace v8;
using namespace node;

void GitTreeEntry::Initialize(Handle<v8::Object> target) {
  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("TreeEntry"));

  NODE_SET_PROTOTYPE_METHOD(tpl, "toBlob", ToBlob);

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

Handle<Value> GitTreeEntry::ToBlob(const Arguments& args) {
  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository is required and must be an Object.")));
  }

  if(args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  ToBlobBaton* baton = new ToBlobBaton();
  baton->request.data = baton;
  baton->error = NULL;
  baton->rawRepo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
  baton->rawEntry = ObjectWrap::Unwrap<GitTreeEntry>(args.This())->GetValue();
  baton->rawBlob = NULL;
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, ToBlobWork, (uv_after_work_cb)ToBlobAfterWork);

  return Undefined();
}
void GitTreeEntry::ToBlobWork(uv_work_t *req) {
  ToBlobBaton* baton = static_cast<ToBlobBaton*>(req->data);

  int returnCode = git_tree_entry_to_object((git_object**)&baton->rawBlob, baton->rawRepo, baton->rawEntry);
  if (returnCode != GIT_OK) {
    baton->error = giterr_last();
  }
}
void GitTreeEntry::ToBlobAfterWork(uv_work_t *req) {
  HandleScope scope;
  ToBlobBaton* baton = static_cast<ToBlobBaton* >(req->data);

  if (baton->error) {
    Local<Value> argv[1] = {
      GitError::WrapError(baton->error)
    };

    TryCatch try_catch;
    baton->callback->Call(Context::GetCurrent()->Global(), 1, argv);
    if (try_catch.HasCaught()) {
      node::FatalException(try_catch);
    }
  } else {

    Handle<Object> blob = GitBlob::constructor_template->NewInstance();
    GitBlob *blobInstance = ObjectWrap::Unwrap<GitBlob>(blob);
    blobInstance->SetValue(baton->rawBlob);

    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      blob
    };

    TryCatch try_catch;
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
    if (try_catch.HasCaught()) {
      node::FatalException(try_catch);
    }
  }
  delete req;
}

Persistent<Function> GitTreeEntry::constructor_template;


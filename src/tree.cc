/*
 * Copyright 2013, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * @author Michael Robinson @codeofinterest <mike@pagesofinterest.net>
 *
 * Dual licensed under the MIT and GPL licenses.
 */

#include <v8.h>
#include <node.h>
#include <vector>

#include "cvv8/v8-convert.hpp"
#include "git2.h"

#include "../include/repo.h"
#include "../include/oid.h"
#include "../include/tree.h"
#include "../include/tree_entry.h"
#include "../include/error.h"

#include "../include/functions/string.h"
#include "../include/functions/utilities.h"

using namespace v8;
using namespace node;
using namespace cvv8;

void GitTree::Initialize (Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Tree"));

  NODE_SET_PROTOTYPE_METHOD(tpl, "lookup", Lookup);
  NODE_SET_PROTOTYPE_METHOD(tpl, "walk", Walk);
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

Handle<Value> GitTree::Lookup(const Arguments& args) {
  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid is required and must be a Object.")));
  }

  if(args.Length() == 1 || !args[1]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository is required and must be a Object.")));
  }

  if(args.Length() == 2 || !args[2]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  LookupBaton* baton = new LookupBaton;
  baton->request.data = baton;
  baton->error = NULL;
  baton->rawTree = ObjectWrap::Unwrap<GitTree>(args.This())->GetValue();
  baton->rawOid = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject())->GetValue();
  baton->rawRepo = ObjectWrap::Unwrap<GitRepo>(args[1]->ToObject())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[2]));

  uv_queue_work(uv_default_loop(), &baton->request, LookupWork, (uv_after_work_cb)LookupAfterWork);

  return Undefined();
}
void GitTree::LookupWork(uv_work_t* req) {
  LookupBaton* baton = static_cast<LookupBaton*>(req->data);

  int returnCode = git_tree_lookup(&baton->rawTree, baton->rawRepo, &baton->rawOid);
  if (returnCode != GIT_OK) {
    baton->error = giterr_last();
  }
}
void GitTree::LookupAfterWork(uv_work_t* req) {
  HandleScope scope;
  LookupBaton* baton = static_cast<LookupBaton* >(req->data);

  if (success(baton->error, baton->callback)) {
    Local<Object> tree = GitTree::constructor_template->NewInstance();
    GitTree *treeInstance = ObjectWrap::Unwrap<GitTree>(tree);
    treeInstance->SetValue(baton->rawTree);

    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      tree
    };

    TryCatch try_catch;
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
    if (try_catch.HasCaught()) {
        node::FatalException(try_catch);
    }
  }
  delete req;
}

Handle<Value> GitTree::Walk(const Arguments& args) {
  HandleScope scope;

  GitTree* tree = ObjectWrap::Unwrap<GitTree>(args.This());

  if (tree->GetValue() == NULL) {
    return ThrowException(Exception::Error(String::New("No tree list to Walk.")));
  }

  if(args.Length() == 0 || !args[0]->IsBoolean()) {
    return ThrowException(Exception::Error(String::New("Blobs only flag is required and must be a Boolean.")));
  }

  if(args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Entry callback is required and must be a Function.")));
  }

  if(args.Length() == 2 || !args[2]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("End callback is required and must be a Function.")));
  }

  WalkBaton* baton = new WalkBaton;
  uv_async_init(uv_default_loop(), &baton->asyncEntry, WalkWorkSendEntry);
  uv_async_init(uv_default_loop(), &baton->asyncEnd, WalkWorkSendEnd);

  uv_mutex_init(&baton->mutex);

  baton->rawTree = tree->GetValue();
  baton->error = NULL;
  baton->blobsOnly = CastFromJS<bool>(args[0]->ToBoolean());
  baton->entryCallback = Persistent<Function>::New(Local<Function>::Cast(args[1]));
  baton->endCallback = Persistent<Function>::New(Local<Function>::Cast(args[2]));

  uv_thread_create(&baton->threadId, WalkWork, baton);

  return Undefined();
}
void GitTree::WalkWork(void* payload) {
  WalkBaton *baton = static_cast<WalkBaton *>(payload);

  int returnCode = git_tree_walk(baton->rawTree, GIT_TREEWALK_PRE, WalkWorkEntry, payload);
  if (returnCode != GIT_OK) {
    baton->error = giterr_last();
    baton->asyncEnd.data = baton;
    uv_async_send(&baton->asyncEnd);
    return;
  }

  baton->asyncEntry.data = baton;
  uv_async_send(&baton->asyncEntry);

  baton->asyncEnd.data = baton;
  uv_async_send(&baton->asyncEnd);
}
int GitTree::WalkWorkEntry(const char* root, const git_tree_entry* entry,
                           void* payload) {
  WalkBaton *baton = static_cast<WalkBaton *>(payload);

  uv_mutex_lock(&baton->mutex);

  if (!baton->blobsOnly) {

    GitTree::WalkEntry* walkEntry = new WalkEntry;
    walkEntry->rawEntry = git_tree_entry_dup(entry);
    walkEntry->root = root;
    baton->rawTreeEntries.push_back(walkEntry);

  } else {
    git_tree_entry* rawEntry = git_tree_entry_dup(entry);
    git_filemode_t fileMode = git_tree_entry_filemode(rawEntry);

    if (fileMode == GIT_FILEMODE_BLOB ||
        fileMode == GIT_FILEMODE_BLOB_EXECUTABLE) {

      GitTree::WalkEntry* walkEntry = new WalkEntry;
      walkEntry->rawEntry = rawEntry;
      walkEntry->root = root;
      baton->rawTreeEntries.push_back(walkEntry);
    }
  }

  uv_mutex_unlock(&baton->mutex);

  if ((unsigned int)baton->rawTreeEntries.size() == (unsigned int)GitTree::WALK_ENTRY_SEND_THRESHOLD) {
    baton->asyncEntry.data = baton;
    uv_async_send(&baton->asyncEntry);
  }

  return GIT_OK;
}
void GitTree::WalkWorkSendEntry(uv_async_t *handle, int status /*UNUSED*/) {
  HandleScope scope;

  WalkBaton *baton = static_cast<WalkBaton *>(handle->data);

  uv_mutex_lock(&baton->mutex);

  if (success(baton->error, baton->entryCallback)) {

    std::vector<Local<Object> > treeEntries;

    for(std::vector<WalkEntry* >::iterator treeEntriesIterator = baton->rawTreeEntries.begin(); treeEntriesIterator != baton->rawTreeEntries.end(); ++treeEntriesIterator) {
      WalkEntry* walkEntry = (*treeEntriesIterator);

      Local<Object> entry = GitTreeEntry::constructor_template->NewInstance();
      GitTreeEntry *entryInstance = ObjectWrap::Unwrap<GitTreeEntry>(entry);
      entryInstance->SetValue(walkEntry->rawEntry);
      entryInstance->SetRoot(walkEntry->root);

      treeEntries.push_back(entry);
    }

    baton->rawTreeEntries.clear();

    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      CastToJS(treeEntries)
    };

    TryCatch try_catch;
    baton->entryCallback->Call(Context::GetCurrent()->Global(), 2, argv);
    if (try_catch.HasCaught()) {
      node::FatalException(try_catch);
    }
  }
  uv_mutex_unlock(&baton->mutex);
}
void GitTree::WalkWorkSendEnd(uv_async_t *handle, int status /*UNUSED*/) {
  WalkBaton *baton = static_cast<WalkBaton *>(handle->data);

  uv_mutex_destroy(&baton->mutex);
  uv_close((uv_handle_t*) &baton->asyncEnd, NULL);
  uv_close((uv_handle_t*) &baton->asyncEntry, NULL);

  Local<Value> argv[1];
  if (baton->error) {
    argv[0] = GitError::WrapError(baton->error);
  } else {
    argv[0] = Local<Value>::New(Null());
  }

  TryCatch try_catch;
  baton->endCallback->Call(Context::GetCurrent()->Global(), 1, argv);
  if (try_catch.HasCaught()) {
      node::FatalException(try_catch);
  }
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
    entryInstance->SetRoot(baton->path.substr(0, baton->path.find_last_of("\\/")));

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

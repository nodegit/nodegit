/*
 * Copyright 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * @author Michael Robinson @codeofinterest <mike@pagesofinterest.net>
 *
 * Dual licensed under the MIT and GPL licenses.
 */

#include <v8.h>
#include <node.h>

#include "cvv8/v8-convert.hpp"
#include "git2.h"

#include "../include/diff_list.h"
#include "../include/error.h"

#include "../include/functions/string.h"
#include "../include/functions/utilities.h"

using namespace v8;
using namespace node;
using namespace cvv8;

GitDiffList::GitDiffList(git_diff_list *raw) {
  this->raw = raw;
}

GitDiffList::~GitDiffList() {
  git_diff_list_free(this->raw);
}

namespace cvv8 {
  template <>
  struct NativeToJS<git_delta_t> : NativeToJS<int32_t> {};
}

void GitDiffList::Initialize(Handle<Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("DiffList"));

  NODE_SET_METHOD(tpl, "treeToTree", TreeToTree);
  NODE_SET_PROTOTYPE_METHOD(tpl, "walk", Walk);

  // Add libgit2 delta types to diff_list object
  Local<Object> libgit2DeltaTypes = Object::New();

  libgit2DeltaTypes->Set(String::NewSymbol("GIT_DELTA_UNMODIFIED"), cvv8::CastToJS(GIT_DELTA_UNMODIFIED), ReadOnly);
  libgit2DeltaTypes->Set(String::NewSymbol("GIT_DELTA_ADDED"), cvv8::CastToJS(GIT_DELTA_ADDED), ReadOnly);
  libgit2DeltaTypes->Set(String::NewSymbol("GIT_DELTA_DELETED"), cvv8::CastToJS(GIT_DELTA_DELETED), ReadOnly);
  libgit2DeltaTypes->Set(String::NewSymbol("GIT_DELTA_MODIFIED"), cvv8::CastToJS(GIT_DELTA_MODIFIED), ReadOnly);
  libgit2DeltaTypes->Set(String::NewSymbol("GIT_DELTA_RENAMED"), cvv8::CastToJS(GIT_DELTA_RENAMED), ReadOnly);
  libgit2DeltaTypes->Set(String::NewSymbol("GIT_DELTA_COPIED"), cvv8::CastToJS(GIT_DELTA_COPIED), ReadOnly);
  libgit2DeltaTypes->Set(String::NewSymbol("GIT_DELTA_IGNORED"), cvv8::CastToJS(GIT_DELTA_IGNORED), ReadOnly);
  libgit2DeltaTypes->Set(String::NewSymbol("GIT_DELTA_UNTRACKED"), cvv8::CastToJS(GIT_DELTA_UNTRACKED), ReadOnly);
  libgit2DeltaTypes->Set(String::NewSymbol("GIT_DELTA_TYPECHANGE"), cvv8::CastToJS(GIT_DELTA_TYPECHANGE), ReadOnly);

  Local<Object> libgit2LineOriginConstants = Object::New();

  // @todo refactor this into something sane
  char _GIT_DIFF_LINE_CONTEXT[2];
  _GIT_DIFF_LINE_CONTEXT[0] = GIT_DIFF_LINE_CONTEXT;
  _GIT_DIFF_LINE_CONTEXT[1] = '\0';
  libgit2LineOriginConstants->Set(String::NewSymbol("GIT_DIFF_LINE_CONTEXT"), cvv8::CastToJS(_GIT_DIFF_LINE_CONTEXT), ReadOnly);

  char _GIT_DIFF_LINE_ADDITION[2];
  _GIT_DIFF_LINE_ADDITION[0] = GIT_DIFF_LINE_ADDITION;
  _GIT_DIFF_LINE_ADDITION[1] = '\0';
  libgit2LineOriginConstants->Set(String::NewSymbol("GIT_DIFF_LINE_ADDITION"), cvv8::CastToJS(_GIT_DIFF_LINE_ADDITION), ReadOnly);

  char _GIT_DIFF_LINE_DELETION[2];
  _GIT_DIFF_LINE_DELETION[0] = GIT_DIFF_LINE_DELETION;
  _GIT_DIFF_LINE_DELETION[1] = '\0';
  libgit2LineOriginConstants->Set(String::NewSymbol("GIT_DIFF_LINE_DELETION"), cvv8::CastToJS(_GIT_DIFF_LINE_DELETION), ReadOnly);

  char _GIT_DIFF_LINE_ADD_EOFNL[2];
  _GIT_DIFF_LINE_ADD_EOFNL[0] = GIT_DIFF_LINE_ADD_EOFNL;
  _GIT_DIFF_LINE_ADD_EOFNL[1] = '\0';
  libgit2LineOriginConstants->Set(String::NewSymbol("GIT_DIFF_LINE_ADD_EOFNL"), cvv8::CastToJS(_GIT_DIFF_LINE_ADD_EOFNL), ReadOnly);

  char _GIT_DIFF_LINE_DEL_EOFNL[2];
  _GIT_DIFF_LINE_DEL_EOFNL[0] = GIT_DIFF_LINE_DEL_EOFNL;
  _GIT_DIFF_LINE_DEL_EOFNL[1] = '\0';
  libgit2LineOriginConstants->Set(String::NewSymbol("GIT_DIFF_LINE_DEL_EOFNL"), cvv8::CastToJS(_GIT_DIFF_LINE_DEL_EOFNL), ReadOnly);

  char _GIT_DIFF_LINE_FILE_HDR[2];
  _GIT_DIFF_LINE_FILE_HDR[0] = GIT_DIFF_LINE_FILE_HDR;
  _GIT_DIFF_LINE_FILE_HDR[1] = '\0';
  libgit2LineOriginConstants->Set(String::NewSymbol("GIT_DIFF_LINE_FILE_HDR"), cvv8::CastToJS(_GIT_DIFF_LINE_FILE_HDR), ReadOnly);

  char _GIT_DIFF_LINE_HUNK_HDR[2];
  _GIT_DIFF_LINE_HUNK_HDR[0] = GIT_DIFF_LINE_HUNK_HDR;
  _GIT_DIFF_LINE_HUNK_HDR[1] = '\0';
  libgit2LineOriginConstants->Set(String::NewSymbol("GIT_DIFF_LINE_HUNK_HDR"), cvv8::CastToJS(_GIT_DIFF_LINE_HUNK_HDR), ReadOnly);

  char _GIT_DIFF_LINE_BINARY[2];
  _GIT_DIFF_LINE_BINARY[0] = GIT_DIFF_LINE_BINARY;
  _GIT_DIFF_LINE_BINARY[1] = '\0';
  libgit2LineOriginConstants->Set(String::NewSymbol("GIT_DIFF_LINE_BINARY"), cvv8::CastToJS(_GIT_DIFF_LINE_BINARY), ReadOnly);

  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  constructor_template->Set(String::NewSymbol("deltaTypes"), libgit2DeltaTypes, ReadOnly);
  constructor_template->Set(String::NewSymbol("lineOriginTypes"), libgit2LineOriginConstants, ReadOnly);
  target->Set(String::NewSymbol("DiffList"), constructor_template);
}

git_diff_list* GitDiffList::GetValue() {
  return this->raw;
}

Handle<Value> GitDiffList::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return ThrowException(Exception::Error(String::New("git_diff_list is required.")));
  }

  GitDiffList* object = new GitDiffList((git_diff_list *) External::Unwrap(args[0]));
  object->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> GitDiffList::TreeToTree(const Arguments& args) {
  HandleScope scope;

  if(args.Length() == 0 || !(args[0]->IsObject() || args[0]->IsString())) {
    return ThrowException(Exception::Error(String::New("Repo is required and must be an Object or String")));
  }

  if(args.Length() == 1 || !(args[1]->IsObject() || args[1]->IsString())) {
    return ThrowException(Exception::Error(String::New("Old Oid/SHA is required and must be an Object or String")));
  }

  if(args.Length() == 2 || !(args[2]->IsObject() || args[2]->IsString())) {
    return ThrowException(Exception::Error(String::New("New Oid/SHA is required and must be an Object or String")));
  }

  if(args.Length() == 3 || !args[3]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  TreeToTreeBaton *baton = new TreeToTreeBaton;
  baton->request.data = baton;
  baton->error = NULL;
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();

  if (args[1]->IsObject()) {
    baton->oldOid = *ObjectWrap::Unwrap<GitOid>(args[1]->ToObject())->GetValue();
  } else {
    baton->oldSha = stringArgToString(args[1]->ToString());
  }

  if (args[2]->IsObject()) {
    baton->newOid = *ObjectWrap::Unwrap<GitOid>(args[2]->ToObject())->GetValue();
  } else {
    baton->newSha = stringArgToString(args[2]->ToString());
  }

  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[3]));

  uv_queue_work(uv_default_loop(), &baton->request, TreeToTreeWork, (uv_after_work_cb)TreeToTreeAfterWork);

  return Undefined();
}

void GitDiffList::TreeToTreeWork(uv_work_t *req) {
  TreeToTreeBaton *baton = static_cast<TreeToTreeBaton *>(req->data);

  // Prepare git_oid's
  git_oid *oldOid = &baton->oldOid;
  if (!baton->oldSha.empty()) {
    int returnCode = git_oid_fromstr(oldOid, baton->oldSha.c_str());
    if (returnCode != GIT_OK) {
      baton->error = giterr_last();
      return;
    }
  }
  git_oid *newOid = &baton->newOid;
  if (!baton->newSha.empty()) {
    int returnCode = git_oid_fromstr(newOid, baton->newSha.c_str());
    if (returnCode != GIT_OK) {
      baton->error = giterr_last();
      return;
    }
  }

  // Get commits
  git_commit* oldCommit = NULL;
  int returnCode = git_commit_lookup(&oldCommit, baton->repo, oldOid);
  if (returnCode != GIT_OK) {
    baton->error = giterr_last();
    return;
  }

  git_commit* newCommit = NULL;
  returnCode = git_commit_lookup(&newCommit, baton->repo, newOid);
  if (returnCode != GIT_OK) {
    baton->error = giterr_last();
    return;
  }

  // Prepare trees
  git_tree* oldTree = NULL;
  returnCode = git_commit_tree(&oldTree, oldCommit);
  if (returnCode != GIT_OK) {
    baton->error = giterr_last();
    return;
  }
  git_tree* newTree = NULL;
  returnCode = git_commit_tree(&newTree, newCommit);
  if (returnCode != GIT_OK) {
    baton->error = giterr_last();
    return;
  }

  baton->rawDiffList = NULL;
  returnCode = git_diff_tree_to_tree(&baton->rawDiffList, baton->repo, oldTree, newTree, NULL);
  if (returnCode != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitDiffList::TreeToTreeAfterWork(uv_work_t *req) {
  HandleScope scope;
  TreeToTreeBaton *baton = static_cast<TreeToTreeBaton *>(req->data);

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
    Handle<Value> argv2[1] = {
      External::New(baton->rawDiffList)
    };

    baton->diffList = ObjectWrap::Unwrap<GitDiffList>(GitDiffList::constructor_template->NewInstance(1, argv2));

    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      baton->diffList->handle_
    };

    TryCatch try_catch;
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
    if (try_catch.HasCaught()) {
      node::FatalException(try_catch);
    }
  }
  delete req;
}

Handle<Value> GitDiffList::Walk(const Arguments& args) {
  HandleScope scope;

  GitDiffList* diffList = ObjectWrap::Unwrap<GitDiffList>(args.This());

  if (diffList->GetValue() == NULL) {
    return ThrowException(Exception::Error(String::New("No diff list to Walk.")));
  }

  if(args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Diff callback is required and must be a Function.")));
  }

  if(args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Hunk callback is required and must be a Function.")));
  }

  if(args.Length() == 2 || !args[2]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Line callback is required and must be a Function.")));
  }

  if(args.Length() == 3 || !args[3]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("End callback is required and must be a Function.")));
  }


  WalkBaton* baton = new WalkBaton;
  uv_async_init(uv_default_loop(), &baton->asyncFile, WalkWorkSendFile);
  uv_async_init(uv_default_loop(), &baton->asyncHunk, WalkWorkSendHunk);
  uv_async_init(uv_default_loop(), &baton->asyncData, WalkWorkSendData);
  uv_async_init(uv_default_loop(), &baton->asyncEnd, WalkWorkSendEnd);

  uv_mutex_init(&baton->mutex);

  baton->rawDiffList = diffList->GetValue();
  diffList->Ref();
  baton->error = NULL;
  baton->fileCallback = Persistent<Function>::New(Local<Function>::Cast(args[0]));
  baton->hunkCallback = Persistent<Function>::New(Local<Function>::Cast(args[1]));
  baton->lineCallback = Persistent<Function>::New(Local<Function>::Cast(args[2]));
  baton->endCallback = Persistent<Function>::New(Local<Function>::Cast(args[3]));

  uv_thread_create(&baton->threadId, WalkWork, baton);

  return Undefined();
}
void GitDiffList::WalkWork(void *payload) {
    WalkBaton *baton = static_cast<WalkBaton *>(payload);

    int returnCode = git_diff_foreach(baton->rawDiffList, WalkWorkFile, WalkWorkHunk, WalkWorkData, payload);
    if (returnCode != GIT_OK) {
      baton->error = giterr_last();
      baton->asyncEnd.data = baton;
      uv_async_send(&baton->asyncEnd);
      return;
    }

    baton->asyncFile.data = baton;
    uv_async_send(&baton->asyncFile);

    baton->asyncEnd.data = baton;
    uv_async_send(&baton->asyncEnd);
}
int GitDiffList::WalkWorkFile(const git_diff_delta *delta, float progress,
                                  void *payload) {
  WalkBaton *baton = static_cast<WalkBaton *>(payload);

  /*
    diff_file_delta
      git_diff_file old_file
        git_oid oid
        const char *path
        git_off_t size
        uint32_t flags
        uint16_t mode

      git_diff_file new_file
      git_delta_t status
      uint32_t similarity
      uint32_t flags
     */

  uv_mutex_lock(&baton->mutex);

  Delta* newDelta = new Delta;
  newDelta->raw = (git_diff_delta*)malloc(sizeof(git_diff_delta));
  memcpy(newDelta->raw, delta, sizeof(git_diff_delta));

  // @todo use combined OID or another less stupid way to index deltas
  std::string key(newDelta->raw->old_file.path);
  key.append(newDelta->raw->new_file.path);
  baton->fileDeltas[key] = newDelta;
  uv_mutex_unlock(&baton->mutex);

  if ((unsigned int)baton->fileDeltas.size() == (unsigned int)GitDiffList::WALK_DELTA_SEND_THRESHHOLD) {
    baton->asyncFile.data = baton;
    uv_async_send(&baton->asyncFile);
  }

  return GIT_OK;
}
int GitDiffList::WalkWorkHunk(const git_diff_delta *delta, const git_diff_range *range, const char *header, size_t header_len, void *payload) {
  return GIT_OK;
}
int GitDiffList::WalkWorkData(const git_diff_delta *delta, const git_diff_range *range,
                                  char line_origin, const char *content, size_t content_len,
                                  void *payload) {
  WalkBaton *baton = static_cast<WalkBaton *>(payload);
  uv_mutex_lock(&baton->mutex);

  GitDiffList::DeltaContent *deltaContent = new GitDiffList::DeltaContent;

  deltaContent->range = (git_diff_range*)malloc(sizeof(git_diff_range));
  memcpy(deltaContent->range, range, sizeof(git_diff_range));

  deltaContent->lineOrigin = line_origin;

  deltaContent->contentLength = content_len;
  deltaContent->content = content;

  std::string key(delta->old_file.path);
  key.append(delta->new_file.path);
  baton->fileDeltas[key]->contents.push_back(deltaContent);

  uv_mutex_unlock(&baton->mutex);

  return GIT_OK;
}
void GitDiffList::WalkWorkSendFile(uv_async_t *handle, int status /*UNUSED*/) {
  HandleScope scope;

  WalkBaton *baton = static_cast<WalkBaton *>(handle->data);

  uv_mutex_lock(&baton->mutex);
  if (success(baton->error, baton->fileCallback)) {

    std::vector<Local<Object> > fileDeltasArray;

    for(std::map<std::string, GitDiffList::Delta* >::iterator iterator = baton->fileDeltas.begin(); iterator != baton->fileDeltas.end(); ++iterator) {

      Local<Object> fileDelta = Object::New();
      GitDiffList::Delta* delta = iterator->second;

      Local<Object> oldFile = Object::New();
      oldFile->Set(String::NewSymbol("path"), String::New(delta->raw->old_file.path));
      fileDelta->Set(String::NewSymbol("oldFile"), oldFile);

      Local<Object> newFile = Object::New();
      newFile->Set(String::NewSymbol("path"), String::New(delta->raw->new_file.path));
      fileDelta->Set(String::NewSymbol("newFile"), newFile);

      std::vector<Local<Object> > deltaContent;
      for(std::vector<GitDiffList::DeltaContent* >::iterator contentIterator = delta->contents.begin(); contentIterator != delta->contents.end(); ++contentIterator) {
        DeltaContent* rawContent = (*contentIterator);
        Local<Object> content = Object::New();

        Local<Object> range = Object::New();
        /*
        int old_start
        int old_lines
        int new_start
        int new_lines
         */
        Local<Object> oldRange = Object::New();
        oldRange->Set(String::New("start"), Integer::New(rawContent->range->old_start));
        oldRange->Set(String::New("lines"), Integer::New(rawContent->range->old_lines));
        range->Set(String::New("old"), oldRange);

        Local<Object> newRange = Object::New();
        newRange->Set(String::New("start"), Integer::New(rawContent->range->new_start));
        newRange->Set(String::New("lines"), Integer::New(rawContent->range->new_lines));
        range->Set(String::New("new"), newRange);

        content->Set(String::New("range"), range);
        content->Set(String::New("content"), String::New(rawContent->content.c_str()));

        // char lineOrigin[2];
        // strncpy(lineOrigin, &rawContent->lineOrigin, 1);
        char lineOrigin[2];
        lineOrigin[0] = rawContent->lineOrigin;
        lineOrigin[1] = '\0';
        // std::string lineOrigin(rawContent->lineOrigin);
        content->Set(String::New("lineOrigin"), String::New(lineOrigin));
        content->Set(String::New("contentLength"), Integer::New(rawContent->contentLength));

        deltaContent.push_back(content);
      }

      fileDelta->Set(String::NewSymbol("content"), cvv8::CastToJS(deltaContent));
      fileDelta->Set(String::NewSymbol("status"), cvv8::CastToJS(delta->raw->status));

      fileDeltasArray.push_back(fileDelta);
    }

    baton->fileDeltas.clear();


    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      cvv8::CastToJS(fileDeltasArray)
    };

    TryCatch try_catch;
    baton->fileCallback->Call(Context::GetCurrent()->Global(), 2, argv);
    if (try_catch.HasCaught()) {
      node::FatalException(try_catch);
    }
  }
  uv_mutex_unlock(&baton->mutex);
}
void GitDiffList::WalkWorkSendHunk(uv_async_t *handle, int status /*UNUSED*/) { }
void GitDiffList::WalkWorkSendData(uv_async_t *handle, int status /*UNUSED*/) { }
void GitDiffList::WalkWorkSendEnd(uv_async_t *handle, int status /*UNUSED*/) {
  WalkBaton *baton = static_cast<WalkBaton *>(handle->data);

  uv_mutex_destroy(&baton->mutex);
  uv_close((uv_handle_t*) &baton->asyncFile, NULL);
  uv_close((uv_handle_t*) &baton->asyncHunk, NULL);
  uv_close((uv_handle_t*) &baton->asyncData, NULL);
  uv_close((uv_handle_t*) &baton->asyncEnd, NULL);

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

Persistent<Function> GitDiffList::constructor_template;

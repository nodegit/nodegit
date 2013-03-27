/*
 * Copyright 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * @author Michael Robinson @codeofinterest <mike@pagesofinterest.net>
 *
 * Dual licensed under the MIT and GPL licenses.
 */

#include <string.h>
#include <v8.h>
#include <node.h>

#include "../vendor/libgit2/include/git2.h"
#include "cvv8/v8-convert.hpp"

#include "../include/reference.h"
#include "../include/sig.h"
#include "../include/repo.h"
#include "../include/oid.h"
#include "../include/tree.h"
#include "../include/commit.h"
#include "../include/error.h"

#include "../include/functions/utilities.h"

using namespace v8;
using namespace cvv8;
using namespace node;

void GitCommit::Initialize(Handle<Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Commit"));

  NODE_SET_PROTOTYPE_METHOD(tpl, "lookup", Lookup);
  NODE_SET_PROTOTYPE_METHOD(tpl, "close", Close);

  NODE_SET_PROTOTYPE_METHOD(tpl, "tree", Tree);
  NODE_SET_PROTOTYPE_METHOD(tpl, "parent", Parent);
  NODE_SET_PROTOTYPE_METHOD(tpl, "fetchDetails", FetchDetails);
  NODE_SET_PROTOTYPE_METHOD(tpl, "fetchDetailsSync", FetchDetailsSync);

  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Commit"), constructor_template);
}

git_commit* GitCommit::GetValue() {
  return this->commit;
}

void GitCommit::SetValue(git_commit* commit) {
  this->commit = commit;
}

void GitCommit::Close() {
  git_commit_free(this->commit);
  this->commit = NULL;
}

int GitCommit::Tree(git_tree** tree) {
  return git_commit_tree(tree, this->commit);
}

Handle<Value> GitCommit::New(const Arguments& args) {
  HandleScope scope;

  GitCommit *commit = new GitCommit();

  commit->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> GitCommit::FetchDetailsSync(const Arguments& args) {
  HandleScope scope;

  GitCommit *commit = ObjectWrap::Unwrap<GitCommit>(args.This());
  git_commit* rawCommit = commit->GetValue();

  GitCommitDetails* details = new GitCommitDetails;
  details->oid = git_commit_id(rawCommit);

  details->sha[GIT_OID_HEXSZ] = '\0';
  git_oid_fmt(details->sha, details->oid);

  details->message = git_commit_message(rawCommit);
  details->time = git_commit_time(rawCommit);
  details->timeOffset = git_commit_time_offset(rawCommit);
  details->committer = git_commit_committer(rawCommit);
  details->author = git_commit_author(rawCommit);
  details->parentCount = git_commit_parentcount(rawCommit);

  int parentCount = details->parentCount;
  while (parentCount > 0) {
    int parentIndex = parentCount -1;
    char sha[GIT_OID_HEXSZ + 1];
    sha[GIT_OID_HEXSZ] = '\0';
    const git_oid *parentOid = git_commit_parent_id(rawCommit, parentIndex);
    git_oid_fmt(sha, parentOid);
    details->parentShas.push_back(sha);
    parentCount--;
  }

  return scope.Close(createCommitDetailsObject(details));
}

Handle<Value> GitCommit::FetchDetails(const Arguments& args) {
  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  FetchDetailsBaton* baton = new FetchDetailsBaton;
  baton->request.data = baton;
  baton->error = NULL;
  baton->details = new GitCommitDetails;
  baton->rawCommit = ObjectWrap::Unwrap<GitCommit>(args.This())->commit;
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, FetchDetailsWork, (uv_after_work_cb)FetchDetailsAfterWork);

  return Undefined();
}

void GitCommit::FetchDetailsWork(uv_work_t *req) {
  FetchDetailsBaton* baton = static_cast<FetchDetailsBaton*>(req->data);

  GitCommitDetails* details = baton->details;
  details->oid = git_commit_id(baton->rawCommit);

  details->sha[GIT_OID_HEXSZ] = '\0';
  git_oid_fmt(details->sha, details->oid);

  details->message = git_commit_message(baton->rawCommit);
  details->time = git_commit_time(baton->rawCommit);
  details->timeOffset = git_commit_time_offset(baton->rawCommit);
  details->committer = git_commit_committer(baton->rawCommit);
  details->author = git_commit_author(baton->rawCommit);
  details->parentCount = git_commit_parentcount(baton->rawCommit);

  int parentCount = details->parentCount;
  while (parentCount > 0) {
    int parentIndex = parentCount -1;
    char sha[GIT_OID_HEXSZ + 1];
    sha[GIT_OID_HEXSZ] = '\0';
    const git_oid *parentOid = git_commit_parent_id(baton->rawCommit, parentIndex);
    git_oid_fmt(sha, parentOid);
    details->parentShas.push_back(sha);
    parentCount--;
  }
}

void GitCommit::FetchDetailsAfterWork(uv_work_t *req) {
  HandleScope scope;

  FetchDetailsBaton* baton = static_cast<FetchDetailsBaton* >(req->data);

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

    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      createCommitDetailsObject(baton->details)
    };

    TryCatch try_catch;
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
    if (try_catch.HasCaught()) {
        node::FatalException(try_catch);
    }
  }
  delete req;
}

Handle<Value> GitCommit::Lookup(const Arguments& args) {
  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repo is required and must be an Object.")));
  }

  if(args.Length() == 1 || !(args[1]->IsObject() || args[1]->IsString())) {
    return ThrowException(Exception::Error(String::New("Oid is required and must be an Object or String")));
  }

  if(args.Length() == 2 || !args[2]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  LookupBaton *baton = new LookupBaton;
  baton->request.data = baton;
  baton->error = NULL;
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();

  if (args[1]->IsObject()) {
    baton->rawOid = ObjectWrap::Unwrap<GitOid>(args[1]->ToObject())->GetValue();
  } else {
    baton->sha = stringArgToString(args[1]->ToString());
  }

  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[2]));

  uv_queue_work(uv_default_loop(), &baton->request, LookupWork, (uv_after_work_cb)LookupAfterWork);

  return Undefined();
}

void GitCommit::LookupWork(uv_work_t *req) {
  LookupBaton *baton = static_cast<LookupBaton *>(req->data);

  git_oid rawOid = baton->rawOid;
  if (!baton->sha.empty()) {
    int returnCode = git_oid_fromstr(&rawOid, baton->sha.c_str());
    if (returnCode != GIT_OK) {
      baton->error = giterr_last();
      return;
    }
  }

  baton->rawCommit = NULL;
  int returnCode = git_commit_lookup(&baton->rawCommit, baton->repo, &rawOid);
  if (returnCode != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitCommit::LookupAfterWork(uv_work_t *req) {
  HandleScope scope;
  LookupBaton *baton = static_cast<LookupBaton *>(req->data);

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

    Local<Object> commit = GitCommit::constructor_template->NewInstance();
    GitCommit *commitInstance = ObjectWrap::Unwrap<GitCommit>(commit);
    commitInstance->SetValue(baton->rawCommit);

    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      commit
    };

    TryCatch try_catch;
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
    if (try_catch.HasCaught()) {
      node::FatalException(try_catch);
    }
  }
  delete req;
}

Handle<Value> GitCommit::Close(const Arguments& args) {
  HandleScope scope;

  GitCommit *commit = ObjectWrap::Unwrap<GitCommit>(args.This());
  commit->Close();

  return scope.Close(Undefined());
}

/**
 * @todo asynchronize
 */
Handle<Value> GitCommit::Tree(const Arguments& args) {
  HandleScope scope;

  GitCommit *commit = ObjectWrap::Unwrap<GitCommit>(args.This());

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Tree is required and must be an Object.")));
  }

  GitTree* g_tree = ObjectWrap::Unwrap<GitTree>(args[0]->ToObject());

  git_tree* tree;
  int err = commit->Tree(&tree);
  g_tree->SetValue(tree);

  return scope.Close( Integer::New(err) );
}

Handle<Value> GitCommit::Parent(const Arguments& args) {
  HandleScope scope;

  if(args.Length() != 2) {
    return ThrowException(Exception::Error(String::New("Position and callback are required")));
  }

  if(!args[0]->IsNumber()) {
    return ThrowException(Exception::Error(String::New("Position is required and must be a Number.")));
  }

  if(!args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  ParentBaton* baton = new ParentBaton();
  baton->request.data = baton;
  baton->rawCommit = ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue();
  baton->error = NULL;
  baton->rawParentCommit = NULL;
  baton->index = args[0]->ToInteger()->Value();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, ParentWork, (uv_after_work_cb)ParentAfterWork);

  return Undefined();
}
void GitCommit::ParentWork(uv_work_t* req) {
  ParentBaton* baton = static_cast<ParentBaton*>(req->data);

  int returnCode = git_commit_parent(&baton->rawParentCommit, baton->rawCommit, baton->index);

  if (returnCode != GIT_OK) {
    baton->error = giterr_last();
  }
}
void GitCommit::ParentAfterWork(uv_work_t* req) {
  HandleScope scope;
  ParentBaton* baton = static_cast<ParentBaton* >(req->data);

  if (success(baton->error, baton->callback)) {
    Local<Object> parent = GitCommit::constructor_template->NewInstance();
    GitCommit *parentInstance = ObjectWrap::Unwrap<GitCommit>(parent);
    parentInstance->SetValue(baton->rawParentCommit);

    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      parent
    };

    TryCatch try_catch;
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
    if (try_catch.HasCaught()) {
        node::FatalException(try_catch);
    }
  }
  delete req;
}

Persistent<Function> GitCommit::constructor_template;

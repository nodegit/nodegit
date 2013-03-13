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
  NODE_SET_PROTOTYPE_METHOD(tpl, "parentSync", ParentSync);
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

Handle<Value> GitCommit::NewInstance() {
  HandleScope scope;

  Local<Object> instance = constructor_template->NewInstance();

  return scope.Close(instance);
}

Handle<Value> GitCommit::FetchDetailsSync(const Arguments& args) {
  HandleScope scope;

  GitCommit *commit = ObjectWrap::Unwrap<GitCommit>(args.This());

  Local<Object> details = Object::New();

  Handle<Object> oid = GitOid::constructor_template->NewInstance();

  char sha[GIT_OID_HEXSZ + 1];
  sha[GIT_OID_HEXSZ] = '\0';
  GitOid *oidInstance = ObjectWrap::Unwrap<GitOid>(oid);
  const git_oid* rawOid = git_commit_id(commit->commit);
  oidInstance->SetValue(*const_cast<git_oid *>(rawOid));
  git_oid_fmt(sha, rawOid);

  details->Set(String::NewSymbol("id"), oid);
  details->Set(String::NewSymbol("sha"), String::New(sha));
  details->Set(String::NewSymbol("message"), cvv8::CastToJS(git_commit_message(commit->commit)));
  details->Set(String::NewSymbol("time"), cvv8::CastToJS(git_commit_time(commit->commit)));
  details->Set(String::NewSymbol("timeOffset"), cvv8::CastToJS(git_commit_time_offset(commit->commit)));

  const git_signature *rawCommitter = git_commit_committer(commit->commit);
  Local<Object> committer = Object::New();
  committer->Set(String::NewSymbol("name"), cvv8::CastToJS(rawCommitter->name));
  committer->Set(String::NewSymbol("email"), cvv8::CastToJS(rawCommitter->email));

  Local<Object> committerWhen = Object::New();
  committerWhen->Set(String::NewSymbol("when"), cvv8::CastToJS(rawCommitter->when.time));
  committerWhen->Set(String::NewSymbol("offset"), cvv8::CastToJS(rawCommitter->when.offset));
  committer->Set(String::NewSymbol("when"), cvv8::CastToJS(committerWhen));

  details->Set(String::NewSymbol("committer"), committer);

  const git_signature* rawAuthor = git_commit_author(commit->commit);
  Local<Object> author = Object::New();
  author->Set(String::NewSymbol("name"), cvv8::CastToJS(rawAuthor->name));
  author->Set(String::NewSymbol("email"), cvv8::CastToJS(rawAuthor->email));

  Local<Object> authorWhen = Object::New();
  authorWhen->Set(String::NewSymbol("when"), cvv8::CastToJS(rawAuthor->when.time));
  authorWhen->Set(String::NewSymbol("offset"), cvv8::CastToJS(rawAuthor->when.offset));
  author->Set(String::NewSymbol("when"), authorWhen);

  details->Set(String::NewSymbol("author"), author);

  int parentCount = git_commit_parentcount(commit->commit);
  std::vector<std::string> parentShas;
  while (parentCount > 0) {
    int parentIndex = parentCount -1;
    char sha[GIT_OID_HEXSZ + 1];
    sha[GIT_OID_HEXSZ] = '\0';
    const git_oid *parentOid = git_commit_parent_id(commit->commit, parentIndex);
    git_oid_fmt(sha, parentOid);
    parentShas.push_back(sha);
    parentCount--;
  }

  details->Set(String::NewSymbol("parentCount"), cvv8::CastToJS(parentCount));
  details->Set(String::NewSymbol("parentShas"), cvv8::CastToJS(parentShas));

  return scope.Close(details);
}

Handle<Value> GitCommit::FetchDetails(const Arguments& args) {
  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  FetchDetailsBaton* baton = new FetchDetailsBaton();
  baton->request.data = baton;
  baton->rawCommit = ObjectWrap::Unwrap<GitCommit>(args.This())->commit;
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, FetchDetailsWork, FetchDetailsAfterWork);

  return Undefined();
}

void GitCommit::FetchDetailsWork(uv_work_t *req) {
  FetchDetailsBaton* baton = static_cast<FetchDetailsBaton*>(req->data);

  baton->oid = git_commit_id(baton->rawCommit);

  baton->sha[GIT_OID_HEXSZ] = '\0';

  git_oid_fmt(baton->sha, baton->oid);

  baton->message = git_commit_message(baton->rawCommit);
  baton->time = git_commit_time(baton->rawCommit);
  baton->timeOffset = git_commit_time_offset(baton->rawCommit);
  baton->committer = git_commit_committer(baton->rawCommit);
  baton->author = git_commit_author(baton->rawCommit);
  baton->parentCount = git_commit_parentcount(baton->rawCommit);

  int parentCount = baton->parentCount;
  while (parentCount > 0) {
    int parentIndex = parentCount -1;
    char sha[GIT_OID_HEXSZ + 1];
    sha[GIT_OID_HEXSZ] = '\0';
    const git_oid *parentOid = git_commit_parent_id(baton->rawCommit, parentIndex);
    git_oid_fmt(sha, parentOid);
    baton->parentShas.push_back(sha);
    parentCount--;
  }
}

void GitCommit::FetchDetailsAfterWork(uv_work_t *req) {
  HandleScope scope;

  FetchDetailsBaton* baton = static_cast<FetchDetailsBaton* >(req->data);
  delete req;

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

    Local<Object> details = Object::New();

    Handle<Object> oid = GitOid::constructor_template->NewInstance();
    GitOid *oidInstance = ObjectWrap::Unwrap<GitOid>(oid);
    oidInstance->SetValue(*const_cast<git_oid *>(baton->oid));

    details->Set(String::NewSymbol("id"), oid);
    details->Set(String::NewSymbol("sha"), String::New(baton->sha));
    details->Set(String::NewSymbol("message"), cvv8::CastToJS(baton->message));
    details->Set(String::NewSymbol("time"), cvv8::CastToJS(baton->time));
    details->Set(String::NewSymbol("timeOffset"), cvv8::CastToJS(baton->timeOffset));

    Local<Object> committer = Object::New();
    committer->Set(String::NewSymbol("name"), cvv8::CastToJS(baton->committer->name));
    committer->Set(String::NewSymbol("email"), cvv8::CastToJS(baton->committer->email));

    Local<Object> committerWhen = Object::New();
    committerWhen->Set(String::NewSymbol("time"), cvv8::CastToJS(baton->committer->when.time));
    committerWhen->Set(String::NewSymbol("offset"), cvv8::CastToJS(baton->committer->when.offset));
    committer->Set(String::NewSymbol("when"), cvv8::CastToJS(committerWhen));

    details->Set(String::NewSymbol("committer"), committer);

    Local<Object> author = Object::New();
    author->Set(String::NewSymbol("name"), cvv8::CastToJS(baton->author->name));
    author->Set(String::NewSymbol("email"), cvv8::CastToJS(baton->author->email));

    Local<Object> authorWhen = Object::New();
    authorWhen->Set(String::NewSymbol("time"), cvv8::CastToJS(baton->author->when.time));
    authorWhen->Set(String::NewSymbol("offset"), cvv8::CastToJS(baton->author->when.offset));
    author->Set(String::NewSymbol("when"), authorWhen);

    details->Set(String::NewSymbol("author"), author);

    details->Set(String::NewSymbol("parentCount"), cvv8::CastToJS(baton->parentCount));
    details->Set(String::NewSymbol("parentShas"), cvv8::CastToJS(baton->parentShas));

    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      details
    };

    TryCatch try_catch;
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
    if (try_catch.HasCaught()) {
        node::FatalException(try_catch);
    }
  }
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

  LookupBaton *baton = new LookupBaton();
  baton->request.data = baton;
  baton->error = NULL;
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();

  if (args[1]->IsObject()) {
    baton->oid = ObjectWrap::Unwrap<GitOid>(args[1]->ToObject())->GetValue();
    baton->sha = NULL;
  } else {
    // Make this less ugly
    String::AsciiValue shaValue(args[1]->ToString());
    char *sha = (char *) malloc(shaValue.length() + 1);
    strcpy(sha, *shaValue);
    baton->sha = sha;
  }
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[2]));

  uv_queue_work(uv_default_loop(), &baton->request, LookupWork, LookupAfterWork);

  return Undefined();
}

void GitCommit::LookupWork(uv_work_t *req) {
  LookupBaton *baton = static_cast<LookupBaton *>(req->data);

  git_oid oid = baton->oid;
  if (baton->sha != NULL) {
    int returnCode = git_oid_fromstr(&oid, baton->sha);
    if (returnCode != GIT_OK) {
      baton->error = giterr_last();
      return;
    }
  }

  baton->rawCommit = NULL;
  int returnCode = git_commit_lookup(&baton->rawCommit, baton->repo, &oid);
  if (returnCode != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitCommit::LookupAfterWork(uv_work_t *req) {
  HandleScope scope;

  LookupBaton *baton = static_cast<LookupBaton *>(req->data);
  delete req;

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
}

Handle<Value> GitCommit::Close(const Arguments& args) {
  HandleScope scope;

  GitCommit *commit = ObjectWrap::Unwrap<GitCommit>(args.This());
  commit->Close();

  return scope.Close(Undefined());
}

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

Handle<Value> GitCommit::ParentSync(const Arguments& args) {
  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsNumber()) {
    return ThrowException(Exception::Error(String::New("Position must be a Number.")));
  }

  GitCommit *commit = ObjectWrap::Unwrap<GitCommit>(args.This());

  git_commit *parentCommitValue ;
  int returnCode = git_commit_parent(&parentCommitValue, commit->commit, args[0]->ToInteger()->Value());

  if (returnCode != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  Local<Object> parent = GitCommit::constructor_template->NewInstance();
  GitCommit *parentCommit = ObjectWrap::Unwrap<GitCommit>(parent);
  parentCommit->SetValue(parentCommitValue);

  return scope.Close(parent);
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
  baton->commit = ObjectWrap::Unwrap<GitCommit>(args.This());
  baton->commit->Ref();
  baton->error = NULL;
  baton->index = args[0]->ToInteger()->Value();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, ParentWork, ParentAfterWork);

  return Undefined();
}

void GitCommit::ParentWork(uv_work_t* req) {
  ParentBaton* baton = static_cast<ParentBaton*>(req->data);

  baton->rawParentCommit = NULL;
  int returnCode = git_commit_parent(&baton->rawParentCommit, baton->commit->commit, baton->index);

  if (returnCode != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitCommit::ParentAfterWork(uv_work_t* req) {
  HandleScope scope;

  ParentBaton* baton = static_cast<ParentBaton* >(req->data);
  delete req;

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
  baton->commit->Unref();
}

Persistent<Function> GitCommit::constructor_template;

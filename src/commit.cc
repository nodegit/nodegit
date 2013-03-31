/*
 * Copyright 2013, Tim Branyen @tbranyen <tim@tabdeveloper.com>
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
#include "../include/signature.h"
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
  NODE_SET_PROTOTYPE_METHOD(tpl, "oid", Oid);
  NODE_SET_PROTOTYPE_METHOD(tpl, "sha", Sha);
  NODE_SET_PROTOTYPE_METHOD(tpl, "message", Message);
  NODE_SET_PROTOTYPE_METHOD(tpl, "time", Time);
  NODE_SET_PROTOTYPE_METHOD(tpl, "offset", Offset);
  NODE_SET_PROTOTYPE_METHOD(tpl, "author", Author);
  NODE_SET_PROTOTYPE_METHOD(tpl, "committer", Committer);
  NODE_SET_PROTOTYPE_METHOD(tpl, "tree", Tree);
  NODE_SET_PROTOTYPE_METHOD(tpl, "parent", Parent);

  NODE_SET_PROTOTYPE_METHOD(tpl, "close", Close);

  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Commit"), constructor_template);
}

git_commit* GitCommit::GetValue() {
  return this->commit;
}

void GitCommit::SetValue(git_commit* commit) {
  this->commit = commit;
}

Handle<Value> GitCommit::New(const Arguments& args) {
  HandleScope scope;

  GitCommit *commit = new GitCommit();
  commit->Wrap(args.This());

  return scope.Close(args.This());
}
Handle<Value> GitCommit::Close(const Arguments& args) {
  HandleScope scope;

  GitCommit *commit = ObjectWrap::Unwrap<GitCommit>(args.This());
  git_commit_free(commit->commit);
  commit->commit = NULL;

  return scope.Close(Undefined());
}
//   details->parentCount = git_commit_parentcount(rawCommit);

//   int parentCount = details->parentCount;
//   while (parentCount > 0) {
//     int parentIndex = parentCount -1;
//     char sha[GIT_OID_HEXSZ + 1];
//     sha[GIT_OID_HEXSZ] = '\0';
//     const git_oid *parentOid = git_commit_parent_id(rawCommit, parentIndex);
//     git_oid_fmt(sha, parentOid);
//     details->parentShas.push_back(sha);
//     parentCount--;
//   }

//   return scope.Close(createCommitDetailsObject(details));
// }

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

  if (!baton->sha.empty()) {
    int returnCode = git_oid_fromstr(&baton->rawOid, baton->sha.c_str());
    if (returnCode != GIT_OK) {
      baton->error = giterr_last();
      return;
    }
  }

  baton->rawCommit = NULL;
  int returnCode = git_commit_lookup(&baton->rawCommit, baton->repo, &baton->rawOid);
  if (returnCode != GIT_OK) {
    baton->error = giterr_last();
  }
}
void GitCommit::LookupAfterWork(uv_work_t *req) {
  HandleScope scope;
  LookupBaton *baton = static_cast<LookupBaton *>(req->data);

  if (success(baton->error, baton->callback)) {
    Local<Object> commit = GitCommit::constructor_template->NewInstance();
    GitCommit *commitInstance = ObjectWrap::Unwrap<GitCommit>(commit);
    commitInstance->SetValue(baton->rawCommit);
    commitInstance->oid = &baton->rawOid;

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

Handle<Value> GitCommit::Oid(const Arguments& args) {
  HandleScope scope;

  Local<Object> oid = GitOid::constructor_template->NewInstance();
  GitOid *oidInstance = ObjectWrap::Unwrap<GitOid>(oid);
  oidInstance->SetValue(*const_cast<git_oid *>(ObjectWrap::Unwrap<GitCommit>(args.This())->oid));

  return scope.Close(oid);
}

Handle<Value> GitCommit::Sha(const Arguments& args) {
  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  ShaBaton* baton = new ShaBaton;
  baton->request.data = baton;
  baton->rawOid = ObjectWrap::Unwrap<GitCommit>(args.This())->oid;
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, ShaWork, (uv_after_work_cb)ShaAfterWork);

  return Undefined();
}
void GitCommit::ShaWork(uv_work_t* req) {
  ShaBaton *baton = static_cast<ShaBaton *>(req->data);

  baton->sha[GIT_OID_HEXSZ] = '\0';

  git_oid_fmt(baton->sha, baton->rawOid);
}
void GitCommit::ShaAfterWork(uv_work_t* req) {
  HandleScope scope;
  ShaBaton *baton = static_cast<ShaBaton *>(req->data);

  Handle<Value> argv[2] = {
    Local<Value>::New(Null()),
    String::New(baton->sha)
  };

  TryCatch try_catch;
  baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  delete req;
}

Handle<Value> GitCommit::Message(const Arguments& args) {
  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  MessageBaton* baton = new MessageBaton;
  baton->request.data = baton;
  baton->rawCommit = ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, MessageWork, (uv_after_work_cb)MessageAfterWork);

  return Undefined();
}
void GitCommit::MessageWork(uv_work_t* req) {
  MessageBaton *baton = static_cast<MessageBaton *>(req->data);

  baton->message = git_commit_message(baton->rawCommit);
}
void GitCommit::MessageAfterWork(uv_work_t* req) {
  HandleScope scope;
  MessageBaton *baton = static_cast<MessageBaton *>(req->data);

  Handle<Value> argv[2] = {
    Local<Value>::New(Null()),
    String::New(baton->message.c_str())
  };

  TryCatch try_catch;
  baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  delete req;
}

Handle<Value> GitCommit::Time(const Arguments& args) {
  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  TimeBaton* baton = new TimeBaton;
  baton->request.data = baton;
  baton->rawCommit = ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, TimeWork, (uv_after_work_cb)TimeAfterWork);

  return Undefined();
}
void GitCommit::TimeWork(uv_work_t* req) {
  TimeBaton *baton = static_cast<TimeBaton *>(req->data);

  baton->time = git_commit_time(baton->rawCommit);
}
void GitCommit::TimeAfterWork(uv_work_t* req) {
  HandleScope scope;
  TimeBaton *baton = static_cast<TimeBaton *>(req->data);

  Handle<Value> argv[2] = {
    Local<Value>::New(Null()),
    Integer::New(baton->time)
  };

  TryCatch try_catch;
  baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  delete req;
}

Handle<Value> GitCommit::Offset(const Arguments& args) {
  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  OffsetBaton* baton = new OffsetBaton;
  baton->request.data = baton;
  baton->rawCommit = ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, OffsetWork, (uv_after_work_cb)OffsetAfterWork);

  return Undefined();
}
void GitCommit::OffsetWork(uv_work_t* req) {
  OffsetBaton *baton = static_cast<OffsetBaton *>(req->data);

  baton->offset = git_commit_time_offset(baton->rawCommit);
}
void GitCommit::OffsetAfterWork(uv_work_t* req) {
  HandleScope scope;
  OffsetBaton *baton = static_cast<OffsetBaton *>(req->data);

  Handle<Value> argv[2] = {
    Local<Value>::New(Null()),
    Integer::New(baton->offset)
  };

  TryCatch try_catch;
  baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  delete req;
}

Handle<Value> GitCommit::Author(const Arguments& args) {
  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  SignatureBaton* baton = new SignatureBaton;
  baton->request.data = baton;
  baton->rawCommit = ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, AuthorWork, (uv_after_work_cb)AuthorAfterWork);

  return Undefined();
}
void GitCommit::AuthorWork(uv_work_t* req) {
  SignatureBaton *baton = static_cast<SignatureBaton *>(req->data);

  baton->rawSignature = git_commit_author(baton->rawCommit);
}
void GitCommit::AuthorAfterWork(uv_work_t* req) {
  HandleScope scope;
  SignatureBaton *baton = static_cast<SignatureBaton *>(req->data);

  Local<Object> signature = GitSignature::constructor_template->NewInstance();
  GitSignature *signatureInstance = ObjectWrap::Unwrap<GitSignature>(signature);
  signatureInstance->SetValue(const_cast<git_signature *>(baton->rawSignature));

  Handle<Value> argv[2] = {
    Local<Value>::New(Null()),
    signature
  };

  TryCatch try_catch;
  baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  delete req;
}

Handle<Value> GitCommit::Committer(const Arguments& args) {
  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  SignatureBaton* baton = new SignatureBaton;
  baton->request.data = baton;
  baton->rawCommit = ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, CommitterWork, (uv_after_work_cb)CommitterAfterWork);

  return Undefined();
}
void GitCommit::CommitterWork(uv_work_t* req) {
  SignatureBaton *baton = static_cast<SignatureBaton *>(req->data);

  baton->rawSignature = git_commit_committer(baton->rawCommit);
}
void GitCommit::CommitterAfterWork(uv_work_t* req) {
  HandleScope scope;
  SignatureBaton *baton = static_cast<SignatureBaton *>(req->data);

  Local<Object> signature = GitSignature::constructor_template->NewInstance();
  GitSignature *signatureInstance = ObjectWrap::Unwrap<GitSignature>(signature);
  signatureInstance->SetValue(const_cast<git_signature *>(baton->rawSignature));

  Handle<Value> argv[2] = {
    Local<Value>::New(Null()),
    signature
  };

  TryCatch try_catch;
  baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  delete req;
}

Handle<Value> GitCommit::Tree(const Arguments& args) {
  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  TreeBaton* baton = new TreeBaton;
  baton->request.data = baton;
  baton->error = NULL;
  baton->rawTree = NULL;
  baton->rawCommit = ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, TreeWork, (uv_after_work_cb)TreeAfterWork);

  return Undefined();
}
void GitCommit::TreeWork(uv_work_t* req) {
  TreeBaton* baton = static_cast<TreeBaton*>(req->data);
  int returnCode = git_commit_tree(&baton->rawTree, baton->rawCommit);
  if (returnCode != GIT_OK) {
    baton->error = giterr_last();
  }
}
void GitCommit::TreeAfterWork(uv_work_t* req) {
  HandleScope scope;
  TreeBaton* baton = static_cast<TreeBaton* >(req->data);

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

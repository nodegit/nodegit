/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/commit.h"
#include "../include/oid.h"
#include "../include/repo.h"
#include "../include/signature.h"
#include "../include/tree.h"

#include "../include/functions/utilities.h"
#include "../include/functions/string.h"

using namespace v8;
using namespace node;

GitCommit::GitCommit(git_commit *raw) {
  this->raw = raw;
}

GitCommit::~GitCommit() {
  git_commit_free(this->raw);
}

void GitCommit::Initialize(Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Commit"));

  NODE_SET_METHOD(tpl, "lookup", Lookup);
  NODE_SET_PROTOTYPE_METHOD(tpl, "oid", Oid);
  NODE_SET_PROTOTYPE_METHOD(tpl, "messageEncoding", MessageEncoding);
  NODE_SET_PROTOTYPE_METHOD(tpl, "message", Message);
  NODE_SET_PROTOTYPE_METHOD(tpl, "time", Time);
  NODE_SET_PROTOTYPE_METHOD(tpl, "offset", Offset);
  NODE_SET_PROTOTYPE_METHOD(tpl, "committer", Committer);
  NODE_SET_PROTOTYPE_METHOD(tpl, "author", Author);
  NODE_SET_PROTOTYPE_METHOD(tpl, "tree", Tree);
  NODE_SET_PROTOTYPE_METHOD(tpl, "treeId", TreeId);
  NODE_SET_PROTOTYPE_METHOD(tpl, "parentCount", ParentCount);
  NODE_SET_PROTOTYPE_METHOD(tpl, "parent", Parent);
  NODE_SET_PROTOTYPE_METHOD(tpl, "parentId", ParentId);
  NODE_SET_PROTOTYPE_METHOD(tpl, "nthGenAncestor", NthGenAncestor);


  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Commit"), constructor_template);
}

Handle<Value> GitCommit::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return ThrowException(Exception::Error(String::New("git_commit is required.")));
  }

  GitCommit* object = new GitCommit((git_commit *) External::Unwrap(args[0]));
  object->Wrap(args.This());

  return scope.Close(args.This());
}

git_commit *GitCommit::GetValue() {
  return this->raw;
}


Handle<Value> GitCommit::Lookup(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository repo is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid id is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  LookupBaton* baton = new LookupBaton;
  baton->error = NULL;
  baton->request.data = baton;
  baton->repoReference = Persistent<Value>::New(args[0]);
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
  baton->idReference = Persistent<Value>::New(args[1]);
  baton->id = ObjectWrap::Unwrap<GitOid>(args[1]->ToObject())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[2]));

  uv_queue_work(uv_default_loop(), &baton->request, LookupWork, (uv_after_work_cb)LookupAfterWork);

  return Undefined();
}

void GitCommit::LookupWork(uv_work_t *req) {
  LookupBaton *baton = static_cast<LookupBaton *>(req->data);
  int commit = git_commit_lookup(
    &baton->commit, 
    baton->repo, 
    baton->id
  );
  if (commit != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitCommit::LookupAfterWork(uv_work_t *req) {
  HandleScope scope;
  LookupBaton *baton = static_cast<LookupBaton *>(req->data);

  TryCatch try_catch;
  if (!baton->error) {
    Handle<Value> argv[1] = { External::New(baton->commit) };
    Handle<Object> commit = GitCommit::constructor_template->NewInstance(1, argv);
    Handle<Value> argv2[2] = {
      Local<Value>::New(Null()),
      commit
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv2);
  } else {
    Handle<Value> argv2[1] = {
      GitError::WrapError(baton->error)
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 1, argv2);
  }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  baton->repoReference.Dispose();
  baton->idReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

Handle<Value> GitCommit::Oid(const Arguments& args) {
  HandleScope scope;

const git_oid *  result = git_commit_id(


    ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
  );


  // XXX need to copy object?
  Handle<Value> argv[1] = { External::New((void *)result) };
  return scope.Close(GitOid::constructor_template->NewInstance(1, argv));
}

Handle<Value> GitCommit::MessageEncoding(const Arguments& args) {
  HandleScope scope;

const char *  result = git_commit_message_encoding(


    ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
  );


  return scope.Close(String::New(result));
}

Handle<Value> GitCommit::Message(const Arguments& args) {
  HandleScope scope;

const char *  result = git_commit_message(


    ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
  );


  return scope.Close(String::New(result));
}

Handle<Value> GitCommit::Time(const Arguments& args) {
  HandleScope scope;

git_time_t  result = git_commit_time(


    ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
  );


  return scope.Close(Number::New(result));
}

Handle<Value> GitCommit::Offset(const Arguments& args) {
  HandleScope scope;

int  result = git_commit_time_offset(


    ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
  );


  return scope.Close(Integer::New(result));
}

Handle<Value> GitCommit::Committer(const Arguments& args) {
  HandleScope scope;

const git_signature *  result = git_commit_committer(


    ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
  );


  // XXX need to copy object?
  Handle<Value> argv[1] = { External::New((void *)result) };
  return scope.Close(GitSignature::constructor_template->NewInstance(1, argv));
}

Handle<Value> GitCommit::Author(const Arguments& args) {
  HandleScope scope;

const git_signature *  result = git_commit_author(


    ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
  );


  // XXX need to copy object?
  Handle<Value> argv[1] = { External::New((void *)result) };
  return scope.Close(GitSignature::constructor_template->NewInstance(1, argv));
}

Handle<Value> GitCommit::Tree(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  TreeBaton* baton = new TreeBaton;
  baton->error = NULL;
  baton->request.data = baton;
  baton->commitReference = Persistent<Value>::New(args.This());
  baton->commit = ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, TreeWork, (uv_after_work_cb)TreeAfterWork);

  return Undefined();
}

void GitCommit::TreeWork(uv_work_t *req) {
  TreeBaton *baton = static_cast<TreeBaton *>(req->data);
  int tree_out = git_commit_tree(
    &baton->tree_out, 
    baton->commit
  );
  if (tree_out != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitCommit::TreeAfterWork(uv_work_t *req) {
  HandleScope scope;
  TreeBaton *baton = static_cast<TreeBaton *>(req->data);

  TryCatch try_catch;
  if (!baton->error) {
    Handle<Value> argv[1] = { External::New(baton->tree_out) };
    Handle<Object> tree_out = GitTree::constructor_template->NewInstance(1, argv);
    Handle<Value> argv2[2] = {
      Local<Value>::New(Null()),
      tree_out
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv2);
  } else {
    Handle<Value> argv2[1] = {
      GitError::WrapError(baton->error)
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 1, argv2);
  }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  baton->commitReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

Handle<Value> GitCommit::TreeId(const Arguments& args) {
  HandleScope scope;

const git_oid *  result = git_commit_tree_id(


    ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
  );


  // XXX need to copy object?
  Handle<Value> argv[1] = { External::New((void *)result) };
  return scope.Close(GitOid::constructor_template->NewInstance(1, argv));
}

Handle<Value> GitCommit::ParentCount(const Arguments& args) {
  HandleScope scope;

unsigned int  result = git_commit_parentcount(


    ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
  );


  return scope.Close(Uint32::New(result));
}

Handle<Value> GitCommit::Parent(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsUint32()) {
    return ThrowException(Exception::Error(String::New("Number n is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  ParentBaton* baton = new ParentBaton;
  baton->error = NULL;
  baton->request.data = baton;
  baton->commitReference = Persistent<Value>::New(args.This());
  baton->commit = ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue();
  baton->nReference = Persistent<Value>::New(args[0]);
  baton->n = (unsigned int) args[0]->ToUint32()->Value();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, ParentWork, (uv_after_work_cb)ParentAfterWork);

  return Undefined();
}

void GitCommit::ParentWork(uv_work_t *req) {
  ParentBaton *baton = static_cast<ParentBaton *>(req->data);
  int out = git_commit_parent(
    &baton->out, 
    baton->commit, 
    baton->n
  );
  if (out != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitCommit::ParentAfterWork(uv_work_t *req) {
  HandleScope scope;
  ParentBaton *baton = static_cast<ParentBaton *>(req->data);

  TryCatch try_catch;
  if (!baton->error) {
    Handle<Value> argv[1] = { External::New(baton->out) };
    Handle<Object> out = GitCommit::constructor_template->NewInstance(1, argv);
    Handle<Value> argv2[2] = {
      Local<Value>::New(Null()),
      out
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv2);
  } else {
    Handle<Value> argv2[1] = {
      GitError::WrapError(baton->error)
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 1, argv2);
  }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  baton->commitReference.Dispose();
  baton->nReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

Handle<Value> GitCommit::ParentId(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsUint32()) {
    return ThrowException(Exception::Error(String::New("Number n is required.")));
  }

const git_oid *  result = git_commit_parent_id(


    ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
, 

  (unsigned int) args[0]->ToUint32()->Value()
  );


  // XXX need to copy object?
  Handle<Value> argv[1] = { External::New((void *)result) };
  return scope.Close(GitOid::constructor_template->NewInstance(1, argv));
}

Handle<Value> GitCommit::NthGenAncestor(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsUint32()) {
    return ThrowException(Exception::Error(String::New("Number n is required.")));
  }
  git_commit * ancestor;

int  result = git_commit_nth_gen_ancestor(

&
    ancestor
, 

    ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
, 

  (unsigned int) args[0]->ToUint32()->Value()
  );

  if (result != GIT_OK) {
    return ThrowException(GitError::WrapError(giterr_last()));
  }

  // XXX need to copy object?
  Handle<Value> argv[1] = { External::New((void *)ancestor) };
  return scope.Close(GitCommit::constructor_template->NewInstance(1, argv));
}


Persistent<Function> GitCommit::constructor_template;

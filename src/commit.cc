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

  NODE_SET_PROTOTYPE_METHOD(tpl, "oid", Oid);
  NODE_SET_PROTOTYPE_METHOD(tpl, "messageEncoding", MessageEncoding);
  NODE_SET_PROTOTYPE_METHOD(tpl, "message", Message);
  NODE_SET_PROTOTYPE_METHOD(tpl, "time", Time);
  NODE_SET_PROTOTYPE_METHOD(tpl, "offset", Offset);
  NODE_SET_PROTOTYPE_METHOD(tpl, "committer", Committer);
  NODE_SET_PROTOTYPE_METHOD(tpl, "author", Author);
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

Handle<Value> GitCommit::New(void *raw) {
  HandleScope scope;
  Handle<Value> argv[1] = { External::New((void *)raw) };
  return scope.Close(GitCommit::constructor_template->NewInstance(1, argv));
}

git_commit *GitCommit::GetValue() {
  return this->raw;
}


Handle<Value> GitCommit::Oid(const Arguments& args) {
  HandleScope scope;
  

  const git_oid * result = git_commit_id(
    ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = GitOid::New((void *)result);
  return scope.Close(to);
}

Handle<Value> GitCommit::MessageEncoding(const Arguments& args) {
  HandleScope scope;
  

  const char * result = git_commit_message_encoding(
    ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = String::New(result);
  return scope.Close(to);
}

Handle<Value> GitCommit::Message(const Arguments& args) {
  HandleScope scope;
  

  const char * result = git_commit_message(
    ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = String::New(result);
  return scope.Close(to);
}

Handle<Value> GitCommit::Time(const Arguments& args) {
  HandleScope scope;
  

  git_time_t result = git_commit_time(
    ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = Number::New(result);
  return scope.Close(to);
}

Handle<Value> GitCommit::Offset(const Arguments& args) {
  HandleScope scope;
  

  int result = git_commit_time_offset(
    ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = Integer::New(result);
  return scope.Close(to);
}

Handle<Value> GitCommit::Committer(const Arguments& args) {
  HandleScope scope;
  

  const git_signature * result = git_commit_committer(
    ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = GitSignature::New((void *)result);
  return scope.Close(to);
}

Handle<Value> GitCommit::Author(const Arguments& args) {
  HandleScope scope;
  

  const git_signature * result = git_commit_author(
    ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = GitSignature::New((void *)result);
  return scope.Close(to);
}

Handle<Value> GitCommit::TreeId(const Arguments& args) {
  HandleScope scope;
  

  const git_oid * result = git_commit_tree_id(
    ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = GitOid::New((void *)result);
  return scope.Close(to);
}

Handle<Value> GitCommit::ParentCount(const Arguments& args) {
  HandleScope scope;
  

  unsigned int result = git_commit_parentcount(
    ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = Uint32::New(result);
  return scope.Close(to);
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
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->commitReference = Persistent<Value>::New(args.This());
  baton->commit = ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue();
  baton->nReference = Persistent<Value>::New(args[0]);
    unsigned int from_n = (unsigned int) args[0]->ToUint32()->Value();
  baton->n = from_n;
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, ParentWork, (uv_after_work_cb)ParentAfterWork);

  return Undefined();
}

void GitCommit::ParentWork(uv_work_t *req) {
  ParentBaton *baton = static_cast<ParentBaton *>(req->data);
  int result = git_commit_parent(
    &baton->out, 
    baton->commit, 
    baton->n
  );
  baton->error_code = result;
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitCommit::ParentAfterWork(uv_work_t *req) {
  HandleScope scope;
  ParentBaton *baton = static_cast<ParentBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    to = GitCommit::New((void *)baton->out);
  Handle<Value> result = to;
    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      result
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
  } else if (baton->error) {
    Handle<Value> argv[1] = {
      Exception::Error(String::New(baton->error->message))
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 1, argv);
  } else {
    baton->callback->Call(Context::GetCurrent()->Global(), 0, NULL);
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

  unsigned int from_n = (unsigned int) args[0]->ToUint32()->Value();

  const git_oid * result = git_commit_parent_id(
    ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
    , from_n
  );

  Handle<Value> to;
    to = GitOid::New((void *)result);
  return scope.Close(to);
}

Handle<Value> GitCommit::NthGenAncestor(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsUint32()) {
    return ThrowException(Exception::Error(String::New("Number n is required.")));
  }

  git_commit *ancestor = NULL;
  unsigned int from_n = (unsigned int) args[0]->ToUint32()->Value();

  int result = git_commit_nth_gen_ancestor(
    &ancestor
    , ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
    , from_n
  );
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  Handle<Value> to;
    to = GitCommit::New((void *)ancestor);
  return scope.Close(to);
}

Persistent<Function> GitCommit::constructor_template;

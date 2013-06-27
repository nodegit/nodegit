/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/branch.h"

#include "../include/functions/string.h"

using namespace v8;
using namespace node;

Branch::Branch(git_branch *raw) {
  this->raw = raw;
}

Branch::~Branch() {
  git_branch_free(this->raw);
}

void Branch::Initialize(Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Branch"));

  NODE_SET_METHOD(tpl, "create", Create);
  NODE_SET_METHOD(tpl, "delete", Delete);
  NODE_SET_METHOD(tpl, "foreach", Foreach);
  NODE_SET_METHOD(tpl, "move", Move);
  NODE_SET_METHOD(tpl, "lookup", Lookup);
  NODE_SET_METHOD(tpl, "name", Name);
  NODE_SET_METHOD(tpl, "upstream", Upstream);
  NODE_SET_METHOD(tpl, "setUpstream", SetUpstream);
  NODE_SET_METHOD(tpl, "upstreamName", UpstreamName);
  NODE_SET_METHOD(tpl, "isHead", IsHead);
  NODE_SET_METHOD(tpl, "remoteName", RemoteName);


  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Branch"), constructor_template);
}

Handle<Value> Branch::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return ThrowException(Exception::Error(String::New("git_branch is required.")));
  }

  Branch* object = new Branch((git_branch *) External::Unwrap(args[0]));
  object->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> Branch::New(void *raw) {
  HandleScope scope;
  Handle<Value> argv[1] = { External::New((void *)raw) };
  return scope.Close(Branch::constructor_template->NewInstance(1, argv));
}

git_branch *Branch::GetValue() {
  return this->raw;
}


Handle<Value> Branch::Create(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository repo is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsString()) {
    return ThrowException(Exception::Error(String::New("String branch_name is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Commit target is required.")));
  }
  if (args.Length() == 3 || !args[3]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number force is required.")));
  }

  git_reference *out = NULL;

  int result = git_branch_create(
    &out
    , ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue()
    , stringArgToString(args[1]->ToString()).c_str()
    , ObjectWrap::Unwrap<GitCommit>(args[2]->ToObject())->GetValue()
    , (int) args[3]->ToInt32()->Value()
  );

  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  Handle<Value> to;
    to = GitReference::New((void *)out);
  return scope.Close(to);
}

Handle<Value> Branch::Delete(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Reference branch is required.")));
  }


  int result = git_branch_delete(
    ObjectWrap::Unwrap<GitReference>(args[0]->ToObject())->GetValue()
  );

  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> Branch::Foreach(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository repo is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsUint32()) {
    return ThrowException(Exception::Error(String::New("Number list_flags is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsObject()) {
    return ThrowException(Exception::Error(String::New("BranchForeachCb branch_cb is required.")));
  }
  if (args.Length() == 3 || !args[3]->IsObject()) {
    return ThrowException(Exception::Error(String::New("void payload is required.")));
  }


  int result = git_branch_foreach(
    ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue()
    , (unsigned int) args[1]->ToUint32()->Value()
    , ObjectWrap::Unwrap<BranchForeachCb>(args[2]->ToObject())->GetValue()
    , ObjectWrap::Unwrap<void>(args[3]->ToObject())->GetValue()
  );

  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> Branch::Move(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Reference branch is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsString()) {
    return ThrowException(Exception::Error(String::New("String new_branch_name is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number force is required.")));
  }

  git_reference *out = NULL;

  int result = git_branch_move(
    &out
    , ObjectWrap::Unwrap<GitReference>(args[0]->ToObject())->GetValue()
    , stringArgToString(args[1]->ToString()).c_str()
    , (int) args[2]->ToInt32()->Value()
  );

  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  Handle<Value> to;
    to = GitReference::New((void *)out);
  return scope.Close(to);
}

Handle<Value> Branch::Lookup(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository repo is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsString()) {
    return ThrowException(Exception::Error(String::New("String branch_name is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsObject()) {
    return ThrowException(Exception::Error(String::New("BranchT branch_type is required.")));
  }

  git_reference *out = NULL;

  int result = git_branch_lookup(
    &out
    , ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue()
    , stringArgToString(args[1]->ToString()).c_str()
    , ObjectWrap::Unwrap<BranchT>(args[2]->ToObject())->GetValue()
  );

  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  Handle<Value> to;
    to = GitReference::New((void *)out);
  return scope.Close(to);
}

Handle<Value> Branch::Name(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Reference ref is required.")));
  }

  const char *out = NULL;

  int result = git_branch_name(
    &out
    , ObjectWrap::Unwrap<GitReference>(args[0]->ToObject())->GetValue()
  );

  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  Handle<Value> to;
    to = String::New(out);
  return scope.Close(to);
}

Handle<Value> Branch::Upstream(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Reference branch is required.")));
  }

  git_reference *out = NULL;

  int result = git_branch_upstream(
    &out
    , ObjectWrap::Unwrap<GitReference>(args[0]->ToObject())->GetValue()
  );

  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  Handle<Value> to;
    to = GitReference::New((void *)out);
  return scope.Close(to);
}

Handle<Value> Branch::SetUpstream(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Reference branch is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsString()) {
    return ThrowException(Exception::Error(String::New("String upstream_name is required.")));
  }


  int result = git_branch_set_upstream(
    ObjectWrap::Unwrap<GitReference>(args[0]->ToObject())->GetValue()
    , stringArgToString(args[1]->ToString()).c_str()
  );

  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> Branch::UpstreamName(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String tracking_branch_name_out is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsUint32()) {
    return ThrowException(Exception::Error(String::New("Number buffer_size is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository repo is required.")));
  }
  if (args.Length() == 3 || !args[3]->IsString()) {
    return ThrowException(Exception::Error(String::New("String canonical_branch_name is required.")));
  }


  int result = git_branch_upstream_name(
    stringArgToString(args[0]->ToString()).c_str()
    , (size_t) args[1]->ToUint32()->Value()
    , ObjectWrap::Unwrap<GitRepo>(args[2]->ToObject())->GetValue()
    , stringArgToString(args[3]->ToString()).c_str()
  );

  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> Branch::IsHead(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Reference branch is required.")));
  }


  int result = git_branch_is_head(
    ObjectWrap::Unwrap<GitReference>(args[0]->ToObject())->GetValue()
  );

  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> Branch::RemoteName(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String remote_name_out is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsUint32()) {
    return ThrowException(Exception::Error(String::New("Number buffer_size is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository repo is required.")));
  }
  if (args.Length() == 3 || !args[3]->IsString()) {
    return ThrowException(Exception::Error(String::New("String canonical_branch_name is required.")));
  }


  int result = git_branch_remote_name(
    stringArgToString(args[0]->ToString()).c_str()
    , (size_t) args[1]->ToUint32()->Value()
    , ObjectWrap::Unwrap<GitRepo>(args[2]->ToObject())->GetValue()
    , stringArgToString(args[3]->ToString()).c_str()
  );

  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Persistent<Function> Branch::constructor_template;

/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/reference.h"
#include "../include/repo.h"
#include "../include/oid.h"
#include "../include/object.h"

#include "../include/functions/utilities.h"
#include "../include/functions/string.h"

using namespace v8;
using namespace node;

GitReference::GitReference(git_reference *raw) {
  this->raw = raw;
}

GitReference::~GitReference() {
  git_reference_free(this->raw);
}

void GitReference::Initialize(Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Reference"));

  NODE_SET_METHOD(tpl, "lookup", Lookup);
  NODE_SET_METHOD(tpl, "oidForName", OidForName);
  NODE_SET_METHOD(tpl, "createSymbolic", CreateSymbolic);
  NODE_SET_METHOD(tpl, "create", Create);
  NODE_SET_PROTOTYPE_METHOD(tpl, "oid", Oid);
  NODE_SET_PROTOTYPE_METHOD(tpl, "name", Name);
  NODE_SET_PROTOTYPE_METHOD(tpl, "type", Type);
  NODE_SET_PROTOTYPE_METHOD(tpl, "resolve", Resolve);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setSymbolicTarget", SetSymbolicTarget);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setTarget", setTarget);
  NODE_SET_PROTOTYPE_METHOD(tpl, "rename", Rename);
  NODE_SET_PROTOTYPE_METHOD(tpl, "delete", Delete);
  NODE_SET_PROTOTYPE_METHOD(tpl, "isBranch", IsBranch);
  NODE_SET_PROTOTYPE_METHOD(tpl, "isRemote", IsRemote);
  NODE_SET_PROTOTYPE_METHOD(tpl, "peel", Peel);
  NODE_SET_METHOD(tpl, "isValidName", IsValidName);


  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Reference"), constructor_template);
}

Handle<Value> GitReference::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return ThrowException(Exception::Error(String::New("git_reference is required.")));
  }

  GitReference* object = new GitReference((git_reference *) External::Unwrap(args[0]));
  object->Wrap(args.This());

  return scope.Close(args.This());
}

git_reference *GitReference::GetValue() {
  return this->raw;
}


Handle<Value> GitReference::Lookup(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository repo is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsString()) {
    return ThrowException(Exception::Error(String::New("String name is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  LookupBaton* baton = new LookupBaton;
  baton->error = NULL;
  baton->request.data = baton;
  baton->repoReference = Persistent<Value>::New(args[0]);
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
  baton->nameReference = Persistent<Value>::New(args[1]);
  String::Utf8Value name(args[1]->ToString());
  baton->name = strdup(*name);
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[2]));

  uv_queue_work(uv_default_loop(), &baton->request, LookupWork, (uv_after_work_cb)LookupAfterWork);

  return Undefined();
}

void GitReference::LookupWork(uv_work_t *req) {
  LookupBaton *baton = static_cast<LookupBaton *>(req->data);
  int result = git_reference_lookup(
    &baton->out, 
    baton->repo, 
    baton->name
  );
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitReference::LookupAfterWork(uv_work_t *req) {
  HandleScope scope;
  LookupBaton *baton = static_cast<LookupBaton *>(req->data);

  TryCatch try_catch;
  if (!baton->error) {
    Handle<Value> argv[1] = { External::New(baton->out) };
    Handle<Object> out = GitReference::constructor_template->NewInstance(1, argv);
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
  baton->repoReference.Dispose();
  baton->nameReference.Dispose();
  baton->callback.Dispose();
  delete baton->name;
  delete baton;
}

Handle<Value> GitReference::OidForName(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository repo is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsString()) {
    return ThrowException(Exception::Error(String::New("String name is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  OidForNameBaton* baton = new OidForNameBaton;
  baton->error = NULL;
  baton->request.data = baton;
  baton->repoReference = Persistent<Value>::New(args[0]);
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
  baton->nameReference = Persistent<Value>::New(args[1]);
  String::Utf8Value name(args[1]->ToString());
  baton->name = strdup(*name);
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[2]));

  uv_queue_work(uv_default_loop(), &baton->request, OidForNameWork, (uv_after_work_cb)OidForNameAfterWork);

  return Undefined();
}

void GitReference::OidForNameWork(uv_work_t *req) {
  OidForNameBaton *baton = static_cast<OidForNameBaton *>(req->data);
  int result = git_reference_name_to_id(
    baton->out, 
    baton->repo, 
    baton->name
  );
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitReference::OidForNameAfterWork(uv_work_t *req) {
  HandleScope scope;
  OidForNameBaton *baton = static_cast<OidForNameBaton *>(req->data);

  TryCatch try_catch;
  if (!baton->error) {
    Handle<Value> argv[1] = { External::New(baton->out) };
    Handle<Object> out = GitOid::constructor_template->NewInstance(1, argv);
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
  baton->repoReference.Dispose();
  baton->nameReference.Dispose();
  baton->callback.Dispose();
  delete baton->name;
  delete baton;
}

Handle<Value> GitReference::CreateSymbolic(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository repo is required.")));
  }

  if (args.Length() == 1 || !args[1]->IsString()) {
    return ThrowException(Exception::Error(String::New("String name is required.")));
  }

  if (args.Length() == 2 || !args[2]->IsString()) {
    return ThrowException(Exception::Error(String::New("String target is required.")));
  }

  if (args.Length() == 3 || !args[3]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number force is required.")));
  }
  git_reference * out;

  int result = git_reference_symbolic_create(
&
    out
, 
    ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue()
, 
    stringArgToString(args[1]->ToString()).c_str()
, 
    stringArgToString(args[2]->ToString()).c_str()
, 
  (int) args[3]->ToInt32()->Value()
  );

  if (result != GIT_OK) {
    return ThrowException(GitError::WrapError(giterr_last()));
  }
  // XXX need to copy object?
  Handle<Value> argv[1] = { External::New((void *)out) };
  return scope.Close(GitReference::constructor_template->NewInstance(1, argv));
}

Handle<Value> GitReference::Create(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository repo is required.")));
  }

  if (args.Length() == 1 || !args[1]->IsString()) {
    return ThrowException(Exception::Error(String::New("String name is required.")));
  }

  if (args.Length() == 2 || !args[2]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid id is required.")));
  }

  if (args.Length() == 3 || !args[3]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number force is required.")));
  }
  git_reference * out;

  int result = git_reference_create(
&
    out
, 
    ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue()
, 
    stringArgToString(args[1]->ToString()).c_str()
, 
    ObjectWrap::Unwrap<GitOid>(args[2]->ToObject())->GetValue()
, 
  (int) args[3]->ToInt32()->Value()
  );

  if (result != GIT_OK) {
    return ThrowException(GitError::WrapError(giterr_last()));
  }
  // XXX need to copy object?
  Handle<Value> argv[1] = { External::New((void *)out) };
  return scope.Close(GitReference::constructor_template->NewInstance(1, argv));
}

Handle<Value> GitReference::Oid(const Arguments& args) {
  HandleScope scope;

  const git_oid * result = git_reference_target(

    ObjectWrap::Unwrap<GitReference>(args.This())->GetValue()
  );

  // XXX need to copy object?
  Handle<Value> argv[1] = { External::New((void *)result) };
  return scope.Close(GitOid::constructor_template->NewInstance(1, argv));
}

Handle<Value> GitReference::Name(const Arguments& args) {
  HandleScope scope;

  const char * result = git_reference_symbolic_target(

    ObjectWrap::Unwrap<GitReference>(args.This())->GetValue()
  );

  return scope.Close(String::New(result));
}

Handle<Value> GitReference::Type(const Arguments& args) {
  HandleScope scope;

  git_ref_t result = git_reference_type(

    ObjectWrap::Unwrap<GitReference>(args.This())->GetValue()
  );

  return scope.Close(Number::New(result));
}

Handle<Value> GitReference::Resolve(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  ResolveBaton* baton = new ResolveBaton;
  baton->error = NULL;
  baton->request.data = baton;
  baton->refReference = Persistent<Value>::New(args.This());
  baton->ref = ObjectWrap::Unwrap<GitReference>(args.This())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, ResolveWork, (uv_after_work_cb)ResolveAfterWork);

  return Undefined();
}

void GitReference::ResolveWork(uv_work_t *req) {
  ResolveBaton *baton = static_cast<ResolveBaton *>(req->data);
  int result = git_reference_resolve(
    &baton->out, 
    baton->ref
  );
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitReference::ResolveAfterWork(uv_work_t *req) {
  HandleScope scope;
  ResolveBaton *baton = static_cast<ResolveBaton *>(req->data);

  TryCatch try_catch;
  if (!baton->error) {
    Handle<Value> argv[1] = { External::New(baton->out) };
    Handle<Object> out = GitReference::constructor_template->NewInstance(1, argv);
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
  baton->refReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

Handle<Value> GitReference::SetSymbolicTarget(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String target is required.")));
  }
  git_reference * out;

  int result = git_reference_symbolic_set_target(
&
    out
, 
    ObjectWrap::Unwrap<GitReference>(args.This())->GetValue()
, 
    stringArgToString(args[0]->ToString()).c_str()
  );

  if (result != GIT_OK) {
    return ThrowException(GitError::WrapError(giterr_last()));
  }
  // XXX need to copy object?
  Handle<Value> argv[1] = { External::New((void *)out) };
  return scope.Close(GitReference::constructor_template->NewInstance(1, argv));
}

Handle<Value> GitReference::setTarget(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid id is required.")));
  }
  git_reference * out;

  int result = git_reference_set_target(
&
    out
, 
    ObjectWrap::Unwrap<GitReference>(args.This())->GetValue()
, 
    ObjectWrap::Unwrap<GitOid>(args[0]->ToObject())->GetValue()
  );

  if (result != GIT_OK) {
    return ThrowException(GitError::WrapError(giterr_last()));
  }
  // XXX need to copy object?
  Handle<Value> argv[1] = { External::New((void *)out) };
  return scope.Close(GitReference::constructor_template->NewInstance(1, argv));
}

Handle<Value> GitReference::Rename(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String new_name is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number force is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  RenameBaton* baton = new RenameBaton;
  baton->error = NULL;
  baton->request.data = baton;
  baton->refReference = Persistent<Value>::New(args.This());
  baton->ref = ObjectWrap::Unwrap<GitReference>(args.This())->GetValue();
  baton->new_nameReference = Persistent<Value>::New(args[0]);
  String::Utf8Value new_name(args[0]->ToString());
  baton->new_name = strdup(*new_name);
  baton->forceReference = Persistent<Value>::New(args[1]);
  baton->force = (int) args[1]->ToInt32()->Value();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[2]));

  uv_queue_work(uv_default_loop(), &baton->request, RenameWork, (uv_after_work_cb)RenameAfterWork);

  return Undefined();
}

void GitReference::RenameWork(uv_work_t *req) {
  RenameBaton *baton = static_cast<RenameBaton *>(req->data);
  int result = git_reference_rename(
    &baton->out, 
    baton->ref, 
    baton->new_name, 
    baton->force
  );
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitReference::RenameAfterWork(uv_work_t *req) {
  HandleScope scope;
  RenameBaton *baton = static_cast<RenameBaton *>(req->data);

  TryCatch try_catch;
  if (!baton->error) {
    Handle<Value> argv[1] = { External::New(baton->out) };
    Handle<Object> out = GitReference::constructor_template->NewInstance(1, argv);
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
  baton->refReference.Dispose();
  baton->new_nameReference.Dispose();
  baton->forceReference.Dispose();
  baton->callback.Dispose();
  delete baton->new_name;
  delete baton;
}

Handle<Value> GitReference::Delete(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  DeleteBaton* baton = new DeleteBaton;
  baton->error = NULL;
  baton->request.data = baton;
  baton->refReference = Persistent<Value>::New(args.This());
  baton->ref = ObjectWrap::Unwrap<GitReference>(args.This())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, DeleteWork, (uv_after_work_cb)DeleteAfterWork);

  return Undefined();
}

void GitReference::DeleteWork(uv_work_t *req) {
  DeleteBaton *baton = static_cast<DeleteBaton *>(req->data);
  int result = git_reference_delete(
    baton->ref
  );
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitReference::DeleteAfterWork(uv_work_t *req) {
  HandleScope scope;
  DeleteBaton *baton = static_cast<DeleteBaton *>(req->data);

  TryCatch try_catch;
  if (!baton->error) {

    Handle<Value> result = Local<Value>::New(Undefined());
    Handle<Value> argv2[2] = {
      Local<Value>::New(Null()),
      result
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
  baton->refReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

Handle<Value> GitReference::IsBranch(const Arguments& args) {
  HandleScope scope;

  int result = git_reference_is_branch(

    ObjectWrap::Unwrap<GitReference>(args.This())->GetValue()
  );

  if (result != GIT_OK) {
    return ThrowException(GitError::WrapError(giterr_last()));
  }
  return scope.Close(Int32::New(result));
}

Handle<Value> GitReference::IsRemote(const Arguments& args) {
  HandleScope scope;

  int result = git_reference_is_remote(

    ObjectWrap::Unwrap<GitReference>(args.This())->GetValue()
  );

  if (result != GIT_OK) {
    return ThrowException(GitError::WrapError(giterr_last()));
  }
  return scope.Close(Int32::New(result));
}

Handle<Value> GitReference::Peel(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number type is required.")));
  }
  git_object * out;

  int result = git_reference_peel(
&
    out
, 
    ObjectWrap::Unwrap<GitReference>(args.This())->GetValue()
, 
  (git_otype) args[0]->ToInt32()->Value()
  );

  if (result != GIT_OK) {
    return ThrowException(GitError::WrapError(giterr_last()));
  }
  // XXX need to copy object?
  Handle<Value> argv[1] = { External::New((void *)out) };
  return scope.Close(GitObject::constructor_template->NewInstance(1, argv));
}

Handle<Value> GitReference::IsValidName(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String refname is required.")));
  }

  int result = git_reference_is_valid_name(

    stringArgToString(args[0]->ToString()).c_str()
  );

  if (result != GIT_OK) {
    return ThrowException(GitError::WrapError(giterr_last()));
  }
  return scope.Close(Int32::New(result));
}


Persistent<Function> GitReference::constructor_template;

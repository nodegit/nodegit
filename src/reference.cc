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
  NODE_SET_PROTOTYPE_METHOD(tpl, "oid", Oid);
  NODE_SET_PROTOTYPE_METHOD(tpl, "name", Name);
  NODE_SET_PROTOTYPE_METHOD(tpl, "type", Type);
  NODE_SET_PROTOTYPE_METHOD(tpl, "resolve", Resolve);


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

  // XXX FIXME potential GC issue: if the argument gets GCd, the destructor could null out this object.
  // Either ref the argument or copy?
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
  String::Utf8Value name(args[1]->ToString());
  // XXX FIXME remove strdup and use std::string
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
    Handle<Object> object = GitReference::constructor_template->NewInstance(1, argv);
    // free((void *) baton->name);
    Handle<Value> argv2[2] = {
      Local<Value>::New(Null()),
      object
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

  // XXX FIXME potential GC issue: if the argument gets GCd, the destructor could null out this object.
  // Either ref the argument or copy?
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
  String::Utf8Value name(args[1]->ToString());
  // XXX FIXME remove strdup and use std::string
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
    Handle<Object> object = GitOid::constructor_template->NewInstance(1, argv);
    // free((void *) baton->name);
    Handle<Value> argv2[2] = {
      Local<Value>::New(Null()),
      object
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

  baton->callback.Dispose();

  delete baton->name;
  delete baton;
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
    Handle<Object> object = GitReference::constructor_template->NewInstance(1, argv);
    Handle<Value> argv2[2] = {
      Local<Value>::New(Null()),
      object
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

  baton->callback.Dispose();
  delete baton;
}


Persistent<Function> GitReference::constructor_template;

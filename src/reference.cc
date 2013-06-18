/*
 * Copyright 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * @author Michael Robinson @codeofinterest <mike@pagesofinterest.net>
 *
 * Dual licensed under the MIT and GPL licenses.
 */

#include <v8.h>
#include <node.h>
#include <string>

#include "git2.h"

#include "../include/repo.h"
#include "../include/reference.h"
#include "../include/oid.h"
#include "../include/error.h"

#include "../include/functions/string.h"
#include "../include/functions/utilities.h"

using namespace v8;
using namespace node;

void GitReference::Initialize(Handle<Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Reference"));

  NODE_SET_PROTOTYPE_METHOD(tpl, "oid", Oid);
  NODE_SET_PROTOTYPE_METHOD(tpl, "lookup", Lookup);

  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Reference"), constructor_template);
}

git_reference* GitReference::GetValue() {
  return this->ref;
}
void GitReference::SetValue(git_reference *ref) {
  this->ref = ref;
}

Handle<Value> GitReference::New(const Arguments& args) {
  HandleScope scope;

  GitReference *ref = new GitReference();

  ref->Wrap(args.This());

  return args.This();
}

Handle<Value> GitReference::Oid(const Arguments& args) {
  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  OidBaton *baton = new OidBaton;
  baton->request.data = baton;
  baton->error = NULL;
  baton->rawOid = NULL;
  baton->rawRef = ObjectWrap::Unwrap<GitReference>(args.This())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, OidWork, (uv_after_work_cb)OidAfterWork);

  return Undefined();
}
void GitReference::OidWork(uv_work_t* req) {
  OidBaton *baton = static_cast<OidBaton *>(req->data);

  git_ref_t referenceType = git_reference_type(baton->rawRef);

  if (referenceType == GIT_REF_INVALID) {
    giterr_set_str(GITERR_INVALID, "Invalid reference type");
    baton->error = giterr_last();
    return;
  }

  if (referenceType == GIT_REF_SYMBOLIC) {
    int returnCode = git_reference_resolve(&baton->rawRef, baton->rawRef);
    if (returnCode != GIT_OK) {
      baton->error = giterr_last();
      return;
    }
  }

  baton->rawOid = git_reference_target(baton->rawRef);
}
void GitReference::OidAfterWork(uv_work_t* req) {
  HandleScope scope;
  OidBaton *baton = static_cast<OidBaton *>(req->data);

  if (success(baton->error, baton->callback)) {
    git_oid *rawOid = (git_oid *)malloc(sizeof(git_oid));
    git_oid_cpy(rawOid, baton->rawOid);
    Handle<Value> argv[1] = { External::New((void *)rawOid) };
    Handle<Object> oid = GitOid::constructor_template->NewInstance(1, argv);

    Handle<Value> argv2[2] = {
      Local<Value>::New(Null()),
      oid
    };

    TryCatch try_catch;
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv2);
    if (try_catch.HasCaught()) {
      node::FatalException(try_catch);
    }
  }

  delete req;
}

Handle<Value> GitReference::Lookup(const Arguments& args) {
  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repo is required and must be a Object.")));
  }

  if(args.Length() == 1 || !args[1]->IsString()) {
    return ThrowException(Exception::Error(String::New("Name is required and must be a String.")));
  }

  if(args.Length() == 2 || !args[2]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  LookupBaton *baton = new LookupBaton;
  baton->request.data = baton;
  baton->ref = ObjectWrap::Unwrap<GitReference>(args.This());
  baton->ref->Ref();
  baton->error = NULL;
  baton->rawRepo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
  baton->name = stringArgToString(args[1]->ToString());
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[2]));

  uv_queue_work(uv_default_loop(), &baton->request, LookupWork, (uv_after_work_cb)LookupAfterWork);

  return Undefined();
}
void GitReference::LookupWork(uv_work_t *req) {
  LookupBaton *baton = static_cast<LookupBaton *>(req->data);

  baton->rawRef = NULL;
  int returnCode = git_reference_lookup(&baton->rawRef, baton->rawRepo, baton->name.c_str());
  if (returnCode != GIT_OK) {
    baton->error = giterr_last();
  }
}
void GitReference::LookupAfterWork(uv_work_t *req) {
  HandleScope scope;
  LookupBaton *baton = static_cast<LookupBaton *>(req->data);

  if (success(baton->error, baton->callback)) {
    baton->ref->SetValue(baton->rawRef);

    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      baton->ref->handle_
    };

    TryCatch try_catch;
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
    if (try_catch.HasCaught()) {
      node::FatalException(try_catch);
    }
  }

  baton->ref->Unref();
  delete req;
}

Persistent<Function> GitReference::constructor_template;

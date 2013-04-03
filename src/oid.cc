/*
 * Copyright 2013, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * @author Michael Robinson @codeofinterest <mike@pagesofinterest.net>
 *
 * Dual licensed under the MIT and GPL licenses.
 */

#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/oid.h"

#include "../include/functions/utilities.h"
#include "../include/functions/string.h"

using namespace v8;
using namespace node;

void GitOid::Initialize(Handle<Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Oid"));

  NODE_SET_PROTOTYPE_METHOD(tpl, "sha", Sha);
  NODE_SET_PROTOTYPE_METHOD(tpl, "fromString", FromString);

  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Oid"), constructor_template);
}

git_oid GitOid::GetValue() {
  return this->oid;
}
void GitOid::SetValue(git_oid oid) {
  this->oid = oid;
}

Handle<Value> GitOid::New(const Arguments& args) {
  HandleScope scope;

  GitOid* oid = new GitOid();
  oid->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> GitOid::Sha(const Arguments& args) {
  HandleScope scope;

  GitOid* oid = ObjectWrap::Unwrap<GitOid>(args.This());

  char sha[GIT_OID_HEXSZ + 1];
  sha[GIT_OID_HEXSZ] = '\0';
  git_oid rawOid = oid->GetValue();
  git_oid_fmt(sha, const_cast<git_oid* >(&rawOid));

  return scope.Close(String::New(sha));
}

Handle<Value> GitOid::FromString(const Arguments& args) {
  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String is required and must be a String.")));
  }

  if(args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  FromStringBaton* baton = new FromStringBaton;
  baton->request.data = baton;
  baton->error = NULL;
  baton->oid = ObjectWrap::Unwrap<GitOid>(args.This());
  baton->rawOid = baton->oid->GetValue();
  baton->fromString = stringArgToString(args[0]->ToString());
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, FromStringWork, (uv_after_work_cb)FromStringAfterWork);

  return Undefined();
}
void GitOid::FromStringWork(uv_work_t* req) {
  FromStringBaton *baton = static_cast<FromStringBaton *>(req->data);

  int returnCode = git_oid_fromstr(&baton->rawOid, baton->fromString.c_str());
  if (returnCode != GIT_OK) {
    baton->error = giterr_last();
  }
}
void GitOid::FromStringAfterWork(uv_work_t* req) {
  HandleScope scope;
  FromStringBaton *baton = static_cast<FromStringBaton *>(req->data);

  if (success(baton->error, baton->callback)) {

    baton->oid->SetValue(baton->rawOid);

    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      baton->oid->handle_
    };

    TryCatch try_catch;
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
    if (try_catch.HasCaught()) {
      node::FatalException(try_catch);
    }
  }
  delete req;
}

Persistent<Function> GitOid::constructor_template;

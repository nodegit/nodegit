/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/oid.h"

#include "../include/functions/utilities.h"
#include "../include/functions/string.h"

using namespace v8;
using namespace node;

GitOid::GitOid(git_oid *raw) {
  this->raw = raw;
}

GitOid::~GitOid() {
}

void GitOid::Initialize(Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Oid"));

  NODE_SET_METHOD(tpl, "fromString", FromString);
  NODE_SET_PROTOTYPE_METHOD(tpl, "sha", Sha);


  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Oid"), constructor_template);
}

Handle<Value> GitOid::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return ThrowException(Exception::Error(String::New("git_oid is required.")));
  }

  GitOid* object = new GitOid((git_oid *) External::Unwrap(args[0]));
  object->Wrap(args.This());

  return scope.Close(args.This());
}

git_oid *GitOid::GetValue() {
  return this->raw;
}


Handle<Value> GitOid::FromString(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String str is required.")));
  }
  git_oid * out;
  out = (git_oid *)malloc(sizeof(git_oid));

  int result = git_oid_fromstr(


    out
, 

    stringArgToString(args[0]->ToString()).c_str()
  );

  if (result != GIT_OK) {
    return ThrowException(GitError::WrapError(giterr_last()));
  }

  // XXX need to copy object?
  Handle<Value> argv[1] = { External::New((void *)out) };
  return scope.Close(GitOid::constructor_template->NewInstance(1, argv));
}

Handle<Value> GitOid::Sha(const Arguments& args) {
  HandleScope scope;

  char * result = git_oid_allocfmt(


    ObjectWrap::Unwrap<GitOid>(args.This())->GetValue()
  );


  return scope.Close(String::New(result));
}


Persistent<Function> GitOid::constructor_template;

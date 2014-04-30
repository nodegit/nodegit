/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/functions/copy.h"

#include "../include/oid.h"

using namespace v8;
using namespace node;

GitOid::GitOid(git_oid *raw) {
  this->raw = raw;
}

GitOid::~GitOid() {
  free(this->raw);
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

Handle<Value> GitOid::New(void *raw) {
  HandleScope scope;
  Handle<Value> argv[1] = { External::New((void *)raw) };
  return scope.Close(GitOid::constructor_template->NewInstance(1, argv));
}

git_oid *GitOid::GetValue() {
  return this->raw;
}


/**
 * @param {String} str
 * @return {Oid} out
 */
Handle<Value> GitOid::FromString(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String str is required.")));
  }

  git_oid *out = (git_oid *)malloc(sizeof(git_oid));
  const char * from_str;
            String::Utf8Value str(args[0]->ToString());
      from_str = strdup(*str);
      
  int result = git_oid_fromstr(
    out
    , from_str
  );
  free((void *)from_str);
  if (result != GIT_OK) {
    free(out);
    if (giterr_last()) {
      return ThrowException(Exception::Error(String::New(giterr_last()->message)));
    } else {
      return ThrowException(Exception::Error(String::New("Unkown Error")));
    }
  }

  Handle<Value> to;
    if (out != NULL) {
    to = GitOid::New((void *)out);
  } else {
    to = Null();
  }
  return scope.Close(to);
}

/**
 * @return {String} result
 */
Handle<Value> GitOid::Sha(const Arguments& args) {
  HandleScope scope;
  

  char * result = git_oid_allocfmt(
    ObjectWrap::Unwrap<GitOid>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = String::New(result);
  free(result);
  return scope.Close(to);
}

Persistent<Function> GitOid::constructor_template;

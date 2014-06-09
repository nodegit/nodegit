/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <nan.h>
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
  NanScope();

  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanNew<String>("Oid"));

  NODE_SET_METHOD(tpl, "fromString", FromString);
  NODE_SET_PROTOTYPE_METHOD(tpl, "sha", Sha);


  Local<Function> _constructor_template = tpl->GetFunction();
  NanAssignPersistent(constructor_template, _constructor_template);
  target->Set(NanNew<String>("Oid"), _constructor_template);
}

NAN_METHOD(GitOid::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError("git_oid is required.");
  }
  GitOid* object = new GitOid(static_cast<git_oid *>(Handle<External>::Cast(args[0])->Value()));
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitOid::New(void *raw) {
  NanEscapableScope();
  Handle<Value> argv[1] = { NanNew<External>((void *)raw) };
  return NanEscapeScope(NanNew<Function>(GitOid::constructor_template)->NewInstance(1, argv));
}

git_oid *GitOid::GetValue() {
  return this->raw;
}


/**
 * @param {String} str
 * @return {Oid} out
 */
NAN_METHOD(GitOid::FromString) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError("String str is required.");
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
      return NanThrowError(giterr_last()->message);
    } else {
      return NanThrowError("Unknown Error");
    }
  }

  Handle<Value> to;
    if (out != NULL) {
    to = GitOid::New((void *)out);
  } else {
    to = NanNull();
  }
  NanReturnValue(to);
}

/**
 * @return {String} result
 */
NAN_METHOD(GitOid::Sha) {
  NanScope();
  

  char * result = git_oid_allocfmt(
    ObjectWrap::Unwrap<GitOid>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = NanNew<String>(result);
  free(result);
  NanReturnValue(to);
}

Persistent<Function> GitOid::constructor_template;

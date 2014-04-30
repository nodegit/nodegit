/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/functions/copy.h"

#include "../include/signature.h"
#include "../include/time.h"

using namespace v8;
using namespace node;

GitSignature::GitSignature(git_signature *raw) {
  this->raw = raw;
}

GitSignature::~GitSignature() {
  git_signature_free(this->raw);
}

void GitSignature::Initialize(Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Signature"));

  NODE_SET_METHOD(tpl, "create", Create);
  NODE_SET_METHOD(tpl, "now", Now);

  NODE_SET_PROTOTYPE_METHOD(tpl, "name", Name);
  NODE_SET_PROTOTYPE_METHOD(tpl, "email", Email);
  NODE_SET_PROTOTYPE_METHOD(tpl, "time", Time);

  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Signature"), constructor_template);
}

Handle<Value> GitSignature::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return ThrowException(Exception::Error(String::New("git_signature is required.")));
  }

  GitSignature* object = new GitSignature((git_signature *) External::Unwrap(args[0]));
  object->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> GitSignature::New(void *raw) {
  HandleScope scope;
  Handle<Value> argv[1] = { External::New((void *)raw) };
  return scope.Close(GitSignature::constructor_template->NewInstance(1, argv));
}

git_signature *GitSignature::GetValue() {
  return this->raw;
}


/**
 * @param {String} name
 * @param {String} email
 * @param {Number} time
 * @param {Number} offset
 * @return {Signature} out
 */
Handle<Value> GitSignature::Create(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String name is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsString()) {
    return ThrowException(Exception::Error(String::New("String email is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number time is required.")));
  }
  if (args.Length() == 3 || !args[3]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number offset is required.")));
  }

  git_signature * out = 0;
  const char * from_name;
            String::Utf8Value name(args[0]->ToString());
      from_name = strdup(*name);
        const char * from_email;
            String::Utf8Value email(args[1]->ToString());
      from_email = strdup(*email);
        git_time_t from_time;
            from_time = (git_time_t)   args[2]->ToInt32()->Value();
        int from_offset;
            from_offset = (int)   args[3]->ToInt32()->Value();
      
  int result = git_signature_new(
    &out
    , from_name
    , from_email
    , from_time
    , from_offset
  );
  free((void *)from_name);
  free((void *)from_email);
  if (result != GIT_OK) {
    if (giterr_last()) {
      return ThrowException(Exception::Error(String::New(giterr_last()->message)));
    } else {
      return ThrowException(Exception::Error(String::New("Unkown Error")));
    }
  }

  Handle<Value> to;
    if (out != NULL) {
    to = GitSignature::New((void *)out);
  } else {
    to = Null();
  }
  return scope.Close(to);
}

/**
 * @param {String} name
 * @param {String} email
 * @return {Signature} out
 */
Handle<Value> GitSignature::Now(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String name is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsString()) {
    return ThrowException(Exception::Error(String::New("String email is required.")));
  }

  git_signature * out = 0;
  const char * from_name;
            String::Utf8Value name(args[0]->ToString());
      from_name = strdup(*name);
        const char * from_email;
            String::Utf8Value email(args[1]->ToString());
      from_email = strdup(*email);
      
  int result = git_signature_now(
    &out
    , from_name
    , from_email
  );
  free((void *)from_name);
  free((void *)from_email);
  if (result != GIT_OK) {
    if (giterr_last()) {
      return ThrowException(Exception::Error(String::New(giterr_last()->message)));
    } else {
      return ThrowException(Exception::Error(String::New("Unkown Error")));
    }
  }

  Handle<Value> to;
    if (out != NULL) {
    to = GitSignature::New((void *)out);
  } else {
    to = Null();
  }
  return scope.Close(to);
}

Handle<Value> GitSignature::Name(const Arguments& args) {
  HandleScope scope;
    Handle<Value> to;

  const char * name =
    ObjectWrap::Unwrap<GitSignature>(args.This())->GetValue()->name;

    to = String::New(name);
  return scope.Close(to);
}

Handle<Value> GitSignature::Email(const Arguments& args) {
  HandleScope scope;
    Handle<Value> to;

  const char * email =
    ObjectWrap::Unwrap<GitSignature>(args.This())->GetValue()->email;

    to = String::New(email);
  return scope.Close(to);
}

Handle<Value> GitSignature::Time(const Arguments& args) {
  HandleScope scope;
    Handle<Value> to;

  git_time *when =
    &ObjectWrap::Unwrap<GitSignature>(args.This())->GetValue()->when;

    if (when != NULL) {
    when = (git_time *)git_time_dup(when);
  }
  if (when != NULL) {
    to = GitTime::New((void *)when);
  } else {
    to = Null();
  }
  return scope.Close(to);
}

Persistent<Function> GitSignature::constructor_template;

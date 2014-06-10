/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <nan.h>
#include <string.h>

#include "git2.h"

#include "../include/functions/copy.h"

#include "../include/odb_object.h"
#include "../include/wrapper.h"
#include "../include/oid.h"

using namespace v8;
using namespace node;

GitOdbObject::GitOdbObject(git_odb_object *raw) {
  this->raw = raw;
}

GitOdbObject::~GitOdbObject() {
  git_odb_object_free(this->raw);
}

void GitOdbObject::Initialize(Handle<v8::Object> target) {
  NanScope();

  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanNew<String>("OdbObject"));

  NODE_SET_PROTOTYPE_METHOD(tpl, "data", Data);
  NODE_SET_PROTOTYPE_METHOD(tpl, "size", Size);
  NODE_SET_PROTOTYPE_METHOD(tpl, "type", Type);
  NODE_SET_PROTOTYPE_METHOD(tpl, "oid", Oid);


  Local<Function> _constructor_template = tpl->GetFunction();
  NanAssignPersistent(constructor_template, _constructor_template);
  target->Set(NanNew<String>("OdbObject"), _constructor_template);
}

NAN_METHOD(GitOdbObject::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError("git_odb_object is required.");
  }
  GitOdbObject* object = new GitOdbObject(static_cast<git_odb_object *>(Handle<External>::Cast(args[0])->Value()));
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitOdbObject::New(void *raw) {
  NanEscapableScope();
  Handle<Value> argv[1] = { NanNew<External>((void *)raw) };
  return NanEscapeScope(NanNew<Function>(GitOdbObject::constructor_template)->NewInstance(1, argv));
}

git_odb_object *GitOdbObject::GetValue() {
  return this->raw;
}


/**
 * @return {Wrapper} result
 */
NAN_METHOD(GitOdbObject::Data) {
  NanScope();
  

  const void * result = git_odb_object_data(
    ObjectWrap::Unwrap<GitOdbObject>(args.This())->GetValue()
  );

  Handle<Value> to;
    if (result != NULL) {
    to = Wrapper::New((void *)result);
  } else {
    to = NanNull();
  }
  NanReturnValue(to);
}

/**
 * @return {Number} result
 */
NAN_METHOD(GitOdbObject::Size) {
  NanScope();
  

  size_t result = git_odb_object_size(
    ObjectWrap::Unwrap<GitOdbObject>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = NanNew<Uint32>((uint32_t)result);
  NanReturnValue(to);
}

/**
 * @return {Number} result
 */
NAN_METHOD(GitOdbObject::Type) {
  NanScope();
  

  git_otype result = git_odb_object_type(
    ObjectWrap::Unwrap<GitOdbObject>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = NanNew<Int32>((int32_t)result);
  NanReturnValue(to);
}

/**
 * @return {Oid} result
 */
NAN_METHOD(GitOdbObject::Oid) {
  NanScope();
  

  const git_oid * result = git_odb_object_id(
    ObjectWrap::Unwrap<GitOdbObject>(args.This())->GetValue()
  );

  Handle<Value> to;
    if (result != NULL) {
    result = (const git_oid * )git_oid_dup(result);
  }
  if (result != NULL) {
    to = GitOid::New((void *)result);
  } else {
    to = NanNull();
  }
  NanReturnValue(to);
}

Persistent<Function> GitOdbObject::constructor_template;

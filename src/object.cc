/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/functions/copy.h"

#include "../include/object.h"
#include "../include/oid.h"
#include "../include/repo.h"

using namespace v8;
using namespace node;

GitObject::GitObject(git_object *raw) {
  this->raw = raw;
}

GitObject::~GitObject() {
  git_object_free(this->raw);
}

void GitObject::Initialize(Handle<v8::Object> target) {
  NanScope();

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanSymbol("Object"));

  NODE_SET_PROTOTYPE_METHOD(tpl, "oid", Oid);
  NODE_SET_PROTOTYPE_METHOD(tpl, "type", Type);
  NODE_SET_PROTOTYPE_METHOD(tpl, "peel", Peel);

  NanAssignPersistent(FunctionTemplate, constructor_template, tpl);
  target->Set(String::NewSymbol("Object"), tpl->GetFunction());
}

NAN_METHOD(GitObject::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError(String::New("git_object is required."));
  }

  GitObject* object = new GitObject((git_object *) External::Cast(*args[0])->Value());
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitObject::New(void *raw) {
  NanScope();
  Handle<Value> argv[1] = { External::New((void *)raw) };
  Local<Object> instance;
  Local<FunctionTemplate> constructorHandle = NanPersistentToLocal(constructor_template);
  instance = constructorHandle->GetFunction()->NewInstance(1, argv);
  return scope.Close(instance);
}

git_object *GitObject::GetValue() {
  return this->raw;
}


/**
 * @return {Oid} result
 */
NAN_METHOD(GitObject::Oid) {
  NanScope();
  

  const git_oid * result = git_object_id(
    ObjectWrap::Unwrap<GitObject>(args.This())->GetValue()
  );

  Handle<Value> to;
    if (result != NULL) {
    result = (const git_oid * )git_oid_dup(result);
  }
  if (result != NULL) {
    to = GitOid::New((void *)result);
  } else {
    to = Null();
  }
  NanReturnValue(to);
}

/**
 * @return {Number} result
 */
NAN_METHOD(GitObject::Type) {
  NanScope();
  

  git_otype result = git_object_type(
    ObjectWrap::Unwrap<GitObject>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = Number::New(result);
  NanReturnValue(to);
}

#include "../include/functions/copy.h"

/**
 * @param {Number} target_type
 * @param {Object} callback
 */
NAN_METHOD(GitObject::Peel) {
  NanScope();
  if (args.Length() == 0 || !args[0]->IsInt32()) {
    return NanThrowError(String::New("Number target_type is required."));
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError(String::New("Callback is required and must be a Function."));
  }

  PeelBaton* baton = new PeelBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  NanAssignPersistent(Value, baton->objectReference, args.This());
  baton->object = ObjectWrap::Unwrap<GitObject>(args.This())->GetValue();
  NanAssignPersistent(Value, baton->target_typeReference, args[0]);
  git_otype from_target_type;
  from_target_type = (git_otype) args[0]->ToInt32()->Value();
  baton->target_type = from_target_type;
  NanAssignPersistent(Function, baton->callback, Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, PeelWork, (uv_after_work_cb)PeelAfterWork);

  NanReturnUndefined();
}

void GitObject::PeelWork(uv_work_t *req) {
  PeelBaton *baton = static_cast<PeelBaton *>(req->data);
  int result = git_object_peel(
    &baton->peeled, 
    baton->object, 
    baton->target_type
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitObject::PeelAfterWork(uv_work_t *req) {
  NanScope();
  PeelBaton *baton = static_cast<PeelBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->peeled != NULL) {
    to = GitObject::New((void *)baton->peeled);
  } else {
    to = Null();
  }
  Handle<Value> result = to;
    Handle<Value> argv[2] = {
      NanNewLocal< Value >(Null()),
      result
    };
    NanPersistentToLocal(baton->callback)->Call(Context::GetCurrent()->Global(), 2, argv);
  } else {
    if (baton->error) {
      Handle<Value> argv[1] = {
        Exception::Error(String::New(baton->error->message))
      };
      NanPersistentToLocal(baton->callback)->Call(Context::GetCurrent()->Global(), 1, argv);
      if (baton->error->message)
        free((void *)baton->error->message);
      free((void *)baton->error);
    } else {
      NanPersistentToLocal(baton->callback)->Call(Context::GetCurrent()->Global(), 0, NULL);
    }
  }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  baton->objectReference.Dispose();
  baton->target_typeReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

Persistent<FunctionTemplate> GitObject::constructor_template;

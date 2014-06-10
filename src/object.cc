/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <nan.h>
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

  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanNew<String>("Object"));

  NODE_SET_PROTOTYPE_METHOD(tpl, "oid", Oid);
  NODE_SET_PROTOTYPE_METHOD(tpl, "type", Type);
  NODE_SET_PROTOTYPE_METHOD(tpl, "peel", Peel);


  Local<Function> _constructor_template = tpl->GetFunction();
  NanAssignPersistent(constructor_template, _constructor_template);
  target->Set(NanNew<String>("Object"), _constructor_template);
}

NAN_METHOD(GitObject::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError("git_object is required.");
  }
  GitObject* object = new GitObject(static_cast<git_object *>(Handle<External>::Cast(args[0])->Value()));
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitObject::New(void *raw) {
  NanEscapableScope();
  Handle<Value> argv[1] = { NanNew<External>((void *)raw) };
  return NanEscapeScope(NanNew<Function>(GitObject::constructor_template)->NewInstance(1, argv));
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
    to = NanNull();
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
    to = NanNew<Number>(result);
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
    return NanThrowError("Number target_type is required.");
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  PeelBaton* baton = new PeelBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->object = ObjectWrap::Unwrap<GitObject>(args.This())->GetValue();
    git_otype from_target_type;
            from_target_type = (git_otype)   args[0]->ToInt32()->Value();
          baton->target_type = from_target_type;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[1]));
  PeelWorker *worker = new PeelWorker(baton, callback);
  worker->SaveToPersistent("object", args.This());
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("target_type", args[0]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitObject::PeelWorker::Execute() {
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

void GitObject::PeelWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->peeled != NULL) {
    to = GitObject::New((void *)baton->peeled);
  } else {
    to = NanNull();
  }
  Handle<Value> result = to;
    Handle<Value> argv[2] = {
      NanNull(),
      result
    };
    callback->Call(2, argv);
  } else {
    if (baton->error) {
      Handle<Value> argv[1] = {
        NanError(baton->error->message)
      };
      callback->Call(1, argv);
      if (baton->error->message)
        free((void *)baton->error->message);
      free((void *)baton->error);
    } else {
      callback->Call(0, NULL);
    }
      }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  delete baton;
}

Persistent<Function> GitObject::constructor_template;

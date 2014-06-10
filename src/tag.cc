/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <nan.h>
#include <string.h>

#include "git2.h"

#include "../include/functions/copy.h"

#include "../include/tag.h"
#include "../include/oid.h"
#include "../include/repo.h"
#include "../include/object.h"
#include "../include/signature.h"

using namespace v8;
using namespace node;

GitTag::GitTag(git_tag *raw) {
  this->raw = raw;
}

GitTag::~GitTag() {
  git_tag_free(this->raw);
}

void GitTag::Initialize(Handle<v8::Object> target) {
  NanScope();

  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanNew<String>("Tag"));

  NODE_SET_PROTOTYPE_METHOD(tpl, "oid", Oid);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getTarget", GetTarget);
  NODE_SET_PROTOTYPE_METHOD(tpl, "targetId", TargetId);
  NODE_SET_PROTOTYPE_METHOD(tpl, "targetType", TargetType);
  NODE_SET_PROTOTYPE_METHOD(tpl, "name", Name);
  NODE_SET_PROTOTYPE_METHOD(tpl, "tagger", Tagger);
  NODE_SET_PROTOTYPE_METHOD(tpl, "message", Message);
  NODE_SET_PROTOTYPE_METHOD(tpl, "peel", Peel);


  Local<Function> _constructor_template = tpl->GetFunction();
  NanAssignPersistent(constructor_template, _constructor_template);
  target->Set(NanNew<String>("Tag"), _constructor_template);
}

NAN_METHOD(GitTag::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError("git_tag is required.");
  }
  GitTag* object = new GitTag(static_cast<git_tag *>(Handle<External>::Cast(args[0])->Value()));
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitTag::New(void *raw) {
  NanEscapableScope();
  Handle<Value> argv[1] = { NanNew<External>((void *)raw) };
  return NanEscapeScope(NanNew<Function>(GitTag::constructor_template)->NewInstance(1, argv));
}

git_tag *GitTag::GetValue() {
  return this->raw;
}


/**
 * @return {Oid} result
 */
NAN_METHOD(GitTag::Oid) {
  NanScope();
  

  const git_oid * result = git_tag_id(
    ObjectWrap::Unwrap<GitTag>(args.This())->GetValue()
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

#include "../include/functions/copy.h"

/**
 * @param {Object} callback
 */
NAN_METHOD(GitTag::GetTarget) {
  NanScope();
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  GetTargetBaton* baton = new GetTargetBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->tag = ObjectWrap::Unwrap<GitTag>(args.This())->GetValue();

  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[0]));
  GetTargetWorker *worker = new GetTargetWorker(baton, callback);
  worker->SaveToPersistent("tag", args.This());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitTag::GetTargetWorker::Execute() {
  int result = git_tag_target(
    &baton->target_out, 
    baton->tag
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitTag::GetTargetWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->target_out != NULL) {
    to = GitObject::New((void *)baton->target_out);
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

/**
 * @return {Oid} result
 */
NAN_METHOD(GitTag::TargetId) {
  NanScope();
  

  const git_oid * result = git_tag_target_id(
    ObjectWrap::Unwrap<GitTag>(args.This())->GetValue()
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
NAN_METHOD(GitTag::TargetType) {
  NanScope();
  

  git_otype result = git_tag_target_type(
    ObjectWrap::Unwrap<GitTag>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = NanNew<Int32>((int32_t)result);
  NanReturnValue(to);
}

/**
 * @return {String} result
 */
NAN_METHOD(GitTag::Name) {
  NanScope();
  

  const char * result = git_tag_name(
    ObjectWrap::Unwrap<GitTag>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = NanNew<String>(result);
  NanReturnValue(to);
}

/**
 * @return {Signature} result
 */
NAN_METHOD(GitTag::Tagger) {
  NanScope();
  

  const git_signature * result = git_tag_tagger(
    ObjectWrap::Unwrap<GitTag>(args.This())->GetValue()
  );

  Handle<Value> to;
    if (result != NULL) {
    result = (const git_signature * )git_signature_dup(result);
  }
  if (result != NULL) {
    to = GitSignature::New((void *)result);
  } else {
    to = NanNull();
  }
  NanReturnValue(to);
}

/**
 * @return {String} result
 */
NAN_METHOD(GitTag::Message) {
  NanScope();
  

  const char * result = git_tag_message(
    ObjectWrap::Unwrap<GitTag>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = NanNew<String>(result);
  NanReturnValue(to);
}

/**
 * @param {Tag} tag
 * @return {Object} tag_target_out
 */
NAN_METHOD(GitTag::Peel) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return NanThrowError("Tag tag is required.");
  }

  git_object * tag_target_out = 0;
  const git_tag * from_tag;
            from_tag = ObjectWrap::Unwrap<GitTag>(args[0]->ToObject())->GetValue();
      
  int result = git_tag_peel(
    &tag_target_out
    , from_tag
  );
  if (result != GIT_OK) {
    if (giterr_last()) {
      return NanThrowError(giterr_last()->message);
    } else {
      return NanThrowError("Unknown Error");
    }
  }

  Handle<Value> to;
    if (tag_target_out != NULL) {
    to = GitObject::New((void *)tag_target_out);
  } else {
    to = NanNull();
  }
  NanReturnValue(to);
}

Persistent<Function> GitTag::constructor_template;

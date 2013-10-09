/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
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
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Tag"));

  NODE_SET_PROTOTYPE_METHOD(tpl, "oid", Oid);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getTarget", GetTarget);
  NODE_SET_PROTOTYPE_METHOD(tpl, "targetId", TargetId);
  NODE_SET_PROTOTYPE_METHOD(tpl, "targetType", TargetType);
  NODE_SET_PROTOTYPE_METHOD(tpl, "name", Name);
  NODE_SET_PROTOTYPE_METHOD(tpl, "tagger", Tagger);
  NODE_SET_PROTOTYPE_METHOD(tpl, "message", Message);
  NODE_SET_PROTOTYPE_METHOD(tpl, "peel", Peel);


  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Tag"), constructor_template);
}

Handle<Value> GitTag::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return ThrowException(Exception::Error(String::New("git_tag is required.")));
  }

  GitTag* object = new GitTag((git_tag *) External::Unwrap(args[0]));
  object->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> GitTag::New(void *raw) {
  HandleScope scope;
  Handle<Value> argv[1] = { External::New((void *)raw) };
  return scope.Close(GitTag::constructor_template->NewInstance(1, argv));
}

git_tag *GitTag::GetValue() {
  return this->raw;
}


/**
 * @return {Oid} result
 */
Handle<Value> GitTag::Oid(const Arguments& args) {
  HandleScope scope;
  

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
    to = Null();
  }
  return scope.Close(to);
}

#include "../include/functions/copy.h"

/**
 * @param {Object} callback
 */
Handle<Value> GitTag::GetTarget(const Arguments& args) {
  HandleScope scope;
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  GetTargetBaton* baton = new GetTargetBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->tagReference = Persistent<Value>::New(args.This());
  baton->tag = ObjectWrap::Unwrap<GitTag>(args.This())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, GetTargetWork, (uv_after_work_cb)GetTargetAfterWork);

  return Undefined();
}

void GitTag::GetTargetWork(uv_work_t *req) {
  GetTargetBaton *baton = static_cast<GetTargetBaton *>(req->data);
  int result = git_tag_target(
    &baton->target_out, 
    baton->tag
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitTag::GetTargetAfterWork(uv_work_t *req) {
  HandleScope scope;
  GetTargetBaton *baton = static_cast<GetTargetBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->target_out != NULL) {
    to = GitObject::New((void *)baton->target_out);
  } else {
    to = Null();
  }
  Handle<Value> result = to;
    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      result
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
  } else {
    if (baton->error) {
      Handle<Value> argv[1] = {
        Exception::Error(String::New(baton->error->message))
      };
      baton->callback->Call(Context::GetCurrent()->Global(), 1, argv);
      if (baton->error->message)
        free((void *)baton->error->message);
      free((void *)baton->error);
    } else {
      baton->callback->Call(Context::GetCurrent()->Global(), 0, NULL);
    }
      }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  baton->tagReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

/**
 * @return {Oid} result
 */
Handle<Value> GitTag::TargetId(const Arguments& args) {
  HandleScope scope;
  

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
    to = Null();
  }
  return scope.Close(to);
}

/**
 * @return {Number} result
 */
Handle<Value> GitTag::TargetType(const Arguments& args) {
  HandleScope scope;
  

  git_otype result = git_tag_target_type(
    ObjectWrap::Unwrap<GitTag>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = Int32::New(result);
  return scope.Close(to);
}

/**
 * @return {String} result
 */
Handle<Value> GitTag::Name(const Arguments& args) {
  HandleScope scope;
  

  const char * result = git_tag_name(
    ObjectWrap::Unwrap<GitTag>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = String::New(result);
  return scope.Close(to);
}

/**
 * @return {Signature} result
 */
Handle<Value> GitTag::Tagger(const Arguments& args) {
  HandleScope scope;
  

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
    to = Null();
  }
  return scope.Close(to);
}

/**
 * @return {String} result
 */
Handle<Value> GitTag::Message(const Arguments& args) {
  HandleScope scope;
  

  const char * result = git_tag_message(
    ObjectWrap::Unwrap<GitTag>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = String::New(result);
  return scope.Close(to);
}

/**
 * @param {Tag} tag
 * @return {Object} tag_target_out
 */
Handle<Value> GitTag::Peel(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Tag tag is required.")));
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
      return ThrowException(Exception::Error(String::New(giterr_last()->message)));
    } else {
      return ThrowException(Exception::Error(String::New("Unkown Error")));
    }
  }

  Handle<Value> to;
    if (tag_target_out != NULL) {
    to = GitObject::New((void *)tag_target_out);
  } else {
    to = Null();
  }
  return scope.Close(to);
}

Persistent<Function> GitTag::constructor_template;

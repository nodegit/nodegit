/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/tag.h"
#include "../include/oid.h"
#include "../include/repo.h"
#include "../include/object.h"
#include "../include/signature.h"

#include "../include/functions/string.h"

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

  NODE_SET_METHOD(tpl, "lookup", Lookup);
  NODE_SET_PROTOTYPE_METHOD(tpl, "oid", Oid);
  NODE_SET_PROTOTYPE_METHOD(tpl, "target", Target);
  NODE_SET_PROTOTYPE_METHOD(tpl, "targetId", TargetId);
  NODE_SET_PROTOTYPE_METHOD(tpl, "targetType", TargetType);
  NODE_SET_PROTOTYPE_METHOD(tpl, "name", Name);
  NODE_SET_PROTOTYPE_METHOD(tpl, "tagger", Tagger);
  NODE_SET_PROTOTYPE_METHOD(tpl, "message", Message);
  NODE_SET_METHOD(tpl, "create", Create);
  NODE_SET_METHOD(tpl, "createLightweight", CreateLightweight);
  NODE_SET_METHOD(tpl, "delete", Delete);
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


Handle<Value> GitTag::Lookup(const Arguments& args) {
  HandleScope scope;
  
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository repo is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid id is required.")));
  }

  if (args.Length() == 2 || !args[2]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  LookupBaton* baton = new LookupBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->repoReference = Persistent<Value>::New(args[0]);
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
  baton->idReference = Persistent<Value>::New(args[1]);
  baton->id = ObjectWrap::Unwrap<GitOid>(args[1]->ToObject())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[2]));

  uv_queue_work(uv_default_loop(), &baton->request, LookupWork, (uv_after_work_cb)LookupAfterWork);

  return Undefined();
}

void GitTag::LookupWork(uv_work_t *req) {
  LookupBaton *baton = static_cast<LookupBaton *>(req->data);
  int result = git_tag_lookup(
    &baton->out, 
    baton->repo, 
    baton->id
  );
  baton->error_code = result;
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitTag::LookupAfterWork(uv_work_t *req) {
  HandleScope scope;
  LookupBaton *baton = static_cast<LookupBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    to = GitTag::New((void *)baton->out);
  Handle<Value> result = to;
    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      result
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
  } else if (baton->error) {
    Handle<Value> argv[1] = {
      Exception::Error(String::New(baton->error->message))
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 1, argv);
  } else {
    baton->callback->Call(Context::GetCurrent()->Global(), 0, NULL);
  }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  baton->repoReference.Dispose();
  baton->idReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

Handle<Value> GitTag::Oid(const Arguments& args) {
  HandleScope scope;
  

  const git_oid * result = git_tag_id(
    ObjectWrap::Unwrap<GitTag>(args.This())->GetValue()
  );


  Handle<Value> to;
    to = GitOid::New((void *)result);
  return scope.Close(to);
}

Handle<Value> GitTag::Target(const Arguments& args) {
  HandleScope scope;
  
  
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  TargetBaton* baton = new TargetBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->tagReference = Persistent<Value>::New(args.This());
  baton->tag = ObjectWrap::Unwrap<GitTag>(args.This())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, TargetWork, (uv_after_work_cb)TargetAfterWork);

  return Undefined();
}

void GitTag::TargetWork(uv_work_t *req) {
  TargetBaton *baton = static_cast<TargetBaton *>(req->data);
  int result = git_tag_target(
    &baton->target_out, 
    baton->tag
  );
  baton->error_code = result;
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitTag::TargetAfterWork(uv_work_t *req) {
  HandleScope scope;
  TargetBaton *baton = static_cast<TargetBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    to = GitObject::New((void *)baton->target_out);
  Handle<Value> result = to;
    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      result
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
  } else if (baton->error) {
    Handle<Value> argv[1] = {
      Exception::Error(String::New(baton->error->message))
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 1, argv);
  } else {
    baton->callback->Call(Context::GetCurrent()->Global(), 0, NULL);
  }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  baton->tagReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

Handle<Value> GitTag::TargetId(const Arguments& args) {
  HandleScope scope;
  

  const git_oid * result = git_tag_target_id(
    ObjectWrap::Unwrap<GitTag>(args.This())->GetValue()
  );


  Handle<Value> to;
    to = GitOid::New((void *)result);
  return scope.Close(to);
}

Handle<Value> GitTag::TargetType(const Arguments& args) {
  HandleScope scope;
  

  git_otype result = git_tag_target_type(
    ObjectWrap::Unwrap<GitTag>(args.This())->GetValue()
  );


  Handle<Value> to;
    to = Int32::New(result);
  return scope.Close(to);
}

Handle<Value> GitTag::Name(const Arguments& args) {
  HandleScope scope;
  

  const char * result = git_tag_name(
    ObjectWrap::Unwrap<GitTag>(args.This())->GetValue()
  );


  Handle<Value> to;
    to = String::New(result);
  return scope.Close(to);
}

Handle<Value> GitTag::Tagger(const Arguments& args) {
  HandleScope scope;
  

  const git_signature * result = git_tag_tagger(
    ObjectWrap::Unwrap<GitTag>(args.This())->GetValue()
  );


  Handle<Value> to;
    to = GitSignature::New((void *)result);
  return scope.Close(to);
}

Handle<Value> GitTag::Message(const Arguments& args) {
  HandleScope scope;
  

  const char * result = git_tag_message(
    ObjectWrap::Unwrap<GitTag>(args.This())->GetValue()
  );


  Handle<Value> to;
    to = String::New(result);
  return scope.Close(to);
}

Handle<Value> GitTag::Create(const Arguments& args) {
  HandleScope scope;
  
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository repo is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsString()) {
    return ThrowException(Exception::Error(String::New("String tag_name is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Object target is required.")));
  }
  if (args.Length() == 3 || !args[3]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Signature tagger is required.")));
  }
  if (args.Length() == 4 || !args[4]->IsString()) {
    return ThrowException(Exception::Error(String::New("String message is required.")));
  }
  if (args.Length() == 5 || !args[5]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number force is required.")));
  }

  if (args.Length() == 6 || !args[6]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  CreateBaton* baton = new CreateBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->repoReference = Persistent<Value>::New(args[0]);
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
  baton->tag_nameReference = Persistent<Value>::New(args[1]);
  String::Utf8Value tag_name(args[1]->ToString());
  baton->tag_name = strdup(*tag_name);
  baton->targetReference = Persistent<Value>::New(args[2]);
  baton->target = ObjectWrap::Unwrap<GitObject>(args[2]->ToObject())->GetValue();
  baton->taggerReference = Persistent<Value>::New(args[3]);
  baton->tagger = ObjectWrap::Unwrap<GitSignature>(args[3]->ToObject())->GetValue();
  baton->messageReference = Persistent<Value>::New(args[4]);
  String::Utf8Value message(args[4]->ToString());
  baton->message = strdup(*message);
  baton->forceReference = Persistent<Value>::New(args[5]);
  baton->force = (int) args[5]->ToInt32()->Value();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[6]));

  uv_queue_work(uv_default_loop(), &baton->request, CreateWork, (uv_after_work_cb)CreateAfterWork);

  return Undefined();
}

void GitTag::CreateWork(uv_work_t *req) {
  CreateBaton *baton = static_cast<CreateBaton *>(req->data);
  int result = git_tag_create(
    baton->oid, 
    baton->repo, 
    baton->tag_name, 
    baton->target, 
    baton->tagger, 
    baton->message, 
    baton->force
  );
  baton->error_code = result;
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitTag::CreateAfterWork(uv_work_t *req) {
  HandleScope scope;
  CreateBaton *baton = static_cast<CreateBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    to = GitOid::New((void *)baton->oid);
  Handle<Value> result = to;
    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      result
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
  } else if (baton->error) {
    Handle<Value> argv[1] = {
      Exception::Error(String::New(baton->error->message))
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 1, argv);
  } else {
    baton->callback->Call(Context::GetCurrent()->Global(), 0, NULL);
  }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  baton->repoReference.Dispose();
  baton->tag_nameReference.Dispose();
  baton->targetReference.Dispose();
  baton->taggerReference.Dispose();
  baton->messageReference.Dispose();
  baton->forceReference.Dispose();
  baton->callback.Dispose();
  delete baton->tag_name;
  delete baton->message;
  delete baton;
}

Handle<Value> GitTag::CreateLightweight(const Arguments& args) {
  HandleScope scope;
  
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository repo is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsString()) {
    return ThrowException(Exception::Error(String::New("String tag_name is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Object target is required.")));
  }
  if (args.Length() == 3 || !args[3]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number force is required.")));
  }

  if (args.Length() == 4 || !args[4]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  CreateLightweightBaton* baton = new CreateLightweightBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->repoReference = Persistent<Value>::New(args[0]);
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
  baton->tag_nameReference = Persistent<Value>::New(args[1]);
  String::Utf8Value tag_name(args[1]->ToString());
  baton->tag_name = strdup(*tag_name);
  baton->targetReference = Persistent<Value>::New(args[2]);
  baton->target = ObjectWrap::Unwrap<GitObject>(args[2]->ToObject())->GetValue();
  baton->forceReference = Persistent<Value>::New(args[3]);
  baton->force = (int) args[3]->ToInt32()->Value();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[4]));

  uv_queue_work(uv_default_loop(), &baton->request, CreateLightweightWork, (uv_after_work_cb)CreateLightweightAfterWork);

  return Undefined();
}

void GitTag::CreateLightweightWork(uv_work_t *req) {
  CreateLightweightBaton *baton = static_cast<CreateLightweightBaton *>(req->data);
  int result = git_tag_create_lightweight(
    baton->oid, 
    baton->repo, 
    baton->tag_name, 
    baton->target, 
    baton->force
  );
  baton->error_code = result;
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitTag::CreateLightweightAfterWork(uv_work_t *req) {
  HandleScope scope;
  CreateLightweightBaton *baton = static_cast<CreateLightweightBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    to = GitOid::New((void *)baton->oid);
  Handle<Value> result = to;
    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      result
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
  } else if (baton->error) {
    Handle<Value> argv[1] = {
      Exception::Error(String::New(baton->error->message))
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 1, argv);
  } else {
    baton->callback->Call(Context::GetCurrent()->Global(), 0, NULL);
  }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  baton->repoReference.Dispose();
  baton->tag_nameReference.Dispose();
  baton->targetReference.Dispose();
  baton->forceReference.Dispose();
  baton->callback.Dispose();
  delete baton->tag_name;
  delete baton;
}

Handle<Value> GitTag::Delete(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository repo is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsString()) {
    return ThrowException(Exception::Error(String::New("String tag_name is required.")));
  }


  int result = git_tag_delete(
    ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue()
    , stringArgToString(args[1]->ToString()).c_str()
  );

  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> GitTag::Peel(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Tag tag is required.")));
  }

  git_object *tag_target_out = NULL;

  int result = git_tag_peel(
    &tag_target_out
    , ObjectWrap::Unwrap<GitTag>(args[0]->ToObject())->GetValue()
  );

  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  Handle<Value> to;
    to = GitObject::New((void *)tag_target_out);
  return scope.Close(to);
}

Persistent<Function> GitTag::constructor_template;

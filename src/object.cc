/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/object.h"
#include "../include/oid.h"
#include "../include/repo.h"

#include "../include/functions/utilities.h"
#include "../include/functions/string.h"

using namespace v8;
using namespace node;

GitObject::GitObject(git_object *raw) {
  this->raw = raw;
}

GitObject::~GitObject() {
  git_object_free(this->raw);
}

void GitObject::Initialize(Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Object"));

  NODE_SET_METHOD(tpl, "lookup", Lookup);
  NODE_SET_PROTOTYPE_METHOD(tpl, "oid", Oid);
  NODE_SET_PROTOTYPE_METHOD(tpl, "type", Type);
  NODE_SET_PROTOTYPE_METHOD(tpl, "peel", Peel);


  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Object"), constructor_template);
}

Handle<Value> GitObject::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return ThrowException(Exception::Error(String::New("git_object is required.")));
  }

  GitObject* object = new GitObject((git_object *) External::Unwrap(args[0]));
  object->Wrap(args.This());

  return scope.Close(args.This());
}

git_object *GitObject::GetValue() {
  return this->raw;
}


Handle<Value> GitObject::Lookup(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository repo is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid id is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number type is required.")));
  }
  if (args.Length() == 3 || !args[3]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  LookupBaton* baton = new LookupBaton;
  baton->error = NULL;
  baton->request.data = baton;

  // XXX FIXME potential GC issue: if the argument gets GCd, the destructor could null out this object.
  // Either ref the argument or copy?
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
  // XXX FIXME potential GC issue: if the argument gets GCd, the destructor could null out this object.
  // Either ref the argument or copy?
  baton->id = ObjectWrap::Unwrap<GitOid>(args[1]->ToObject())->GetValue();
  baton->type = (git_otype) args[2]->ToInt32()->Value();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[3]));

  uv_queue_work(uv_default_loop(), &baton->request, LookupWork, (uv_after_work_cb)LookupAfterWork);

  return Undefined();
}

void GitObject::LookupWork(uv_work_t *req) {
  LookupBaton *baton = static_cast<LookupBaton *>(req->data);
  int result = git_object_lookup(
    &baton->object, 
    baton->repo, 
    baton->id, 
    baton->type
  );
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitObject::LookupAfterWork(uv_work_t *req) {
  HandleScope scope;
  LookupBaton *baton = static_cast<LookupBaton *>(req->data);

  TryCatch try_catch;
  if (!baton->error) {

    Handle<Value> argv[1] = { External::New(baton->object) };
    Handle<Object> object = GitObject::constructor_template->NewInstance(1, argv);
    Handle<Value> argv2[2] = {
      Local<Value>::New(Null()),
      object
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv2);
  } else {
    Handle<Value> argv2[1] = {
      GitError::WrapError(baton->error)
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 1, argv2);
  }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }

  baton->callback.Dispose();
  delete baton;
}

Handle<Value> GitObject::Oid(const Arguments& args) {
  HandleScope scope;

  const git_oid * result = git_object_id(

    ObjectWrap::Unwrap<GitObject>(args.This())->GetValue()
  );

  // XXX need to copy object?
  Handle<Value> argv[1] = { External::New((void *)result) };
  return scope.Close(GitOid::constructor_template->NewInstance(1, argv));
}

Handle<Value> GitObject::Type(const Arguments& args) {
  HandleScope scope;

  git_otype result = git_object_type(

    ObjectWrap::Unwrap<GitObject>(args.This())->GetValue()
  );

  return scope.Close(Number::New(result));
}

Handle<Value> GitObject::Peel(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number target_type is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  PeelBaton* baton = new PeelBaton;
  baton->error = NULL;
  baton->request.data = baton;

  baton->object = ObjectWrap::Unwrap<GitObject>(args.This())->GetValue();
  baton->target_type = (git_otype) args[0]->ToInt32()->Value();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, PeelWork, (uv_after_work_cb)PeelAfterWork);

  return Undefined();
}

void GitObject::PeelWork(uv_work_t *req) {
  PeelBaton *baton = static_cast<PeelBaton *>(req->data);
  int result = git_object_peel(
    &baton->peeled, 
    baton->object, 
    baton->target_type
  );
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitObject::PeelAfterWork(uv_work_t *req) {
  HandleScope scope;
  PeelBaton *baton = static_cast<PeelBaton *>(req->data);

  TryCatch try_catch;
  if (!baton->error) {

    Handle<Value> argv[1] = { External::New(baton->peeled) };
    Handle<Object> object = GitObject::constructor_template->NewInstance(1, argv);
    Handle<Value> argv2[2] = {
      Local<Value>::New(Null()),
      object
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv2);
  } else {
    Handle<Value> argv2[1] = {
      GitError::WrapError(baton->error)
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 1, argv2);
  }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }

  baton->callback.Dispose();
  delete baton;
}


Persistent<Function> GitObject::constructor_template;

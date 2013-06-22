/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/repo.h"

#include "../include/functions/utilities.h"
#include "../include/functions/string.h"

using namespace v8;
using namespace node;

GitRepo::GitRepo(git_repository *raw) {
  this->raw = raw;
}

GitRepo::~GitRepo() {
  git_repository_free(this->raw);
}

void GitRepo::Initialize(Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Repo"));

  NODE_SET_METHOD(tpl, "open", Open);
  NODE_SET_METHOD(tpl, "init", Init);
  NODE_SET_PROTOTYPE_METHOD(tpl, "path", Path);
  NODE_SET_PROTOTYPE_METHOD(tpl, "workdir", Workdir);

  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Repo"), constructor_template);
}

Handle<Value> GitRepo::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return ThrowException(Exception::Error(String::New("git_repository is required.")));
  }

  GitRepo* object = new GitRepo((git_repository *) External::Unwrap(args[0]));
  object->Wrap(args.This());

  return scope.Close(args.This());
}

git_repository *GitRepo::GetValue() {
  return this->raw;
}


Handle<Value> GitRepo::Open(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  OpenBaton* baton = new OpenBaton;
  baton->error = NULL;
  baton->request.data = baton;

  String::Utf8Value path(args[0]->ToString());
  // XXX FIXME remove strdup and use std::string
  baton->path = strdup(*path);
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, OpenWork, (uv_after_work_cb)OpenAfterWork);

  return Undefined();
}

void GitRepo::OpenWork(uv_work_t *req) {
  OpenBaton *baton = static_cast<OpenBaton *>(req->data);
  int result = git_repository_open(
    &baton->out, 
    baton->path
  );
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitRepo::OpenAfterWork(uv_work_t *req) {
  HandleScope scope;
  OpenBaton *baton = static_cast<OpenBaton *>(req->data);

  TryCatch try_catch;
  if (!baton->error) {

    Handle<Value> argv[1] = { External::New(baton->out) };
    Handle<Object> object = GitRepo::constructor_template->NewInstance(1, argv);
    // free((void *) baton->path);
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

  delete baton->path;
  delete baton;
}

Handle<Value> GitRepo::Init(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsBoolean()) {
    return ThrowException(Exception::Error(String::New("Boolean is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  InitBaton* baton = new InitBaton;
  baton->error = NULL;
  baton->request.data = baton;

  String::Utf8Value path(args[0]->ToString());
  // XXX FIXME remove strdup and use std::string
  baton->path = strdup(*path);
  baton->is_bare = args[1]->ToBoolean()->Value();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[2]));

  uv_queue_work(uv_default_loop(), &baton->request, InitWork, (uv_after_work_cb)InitAfterWork);

  return Undefined();
}

void GitRepo::InitWork(uv_work_t *req) {
  InitBaton *baton = static_cast<InitBaton *>(req->data);
  int result = git_repository_init(
    &baton->out, 
    baton->path, 
    baton->is_bare
  );
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitRepo::InitAfterWork(uv_work_t *req) {
  HandleScope scope;
  InitBaton *baton = static_cast<InitBaton *>(req->data);

  TryCatch try_catch;
  if (!baton->error) {

    Handle<Value> argv[1] = { External::New(baton->out) };
    Handle<Object> object = GitRepo::constructor_template->NewInstance(1, argv);
    // free((void *) baton->path);
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

  delete baton->path;
  delete baton;
}

Handle<Value> GitRepo::Path(const Arguments& args) {
  HandleScope scope;

  const char * result = git_repository_path(
    ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue()
  );

  return scope.Close(String::New(result));
}

Handle<Value> GitRepo::Workdir(const Arguments& args) {
  HandleScope scope;

  const char * result = git_repository_workdir(
    ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue()
  );

  return scope.Close(String::New(result));
}

Persistent<Function> GitRepo::constructor_template;

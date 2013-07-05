/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/functions/copy.h"

#include "../include/repo.h"
#include "../include/oid.h"
#include "../include/commit.h"
#include "../include/blob.h"
#include "../include/object.h"
#include "../include/reference.h"
#include "../include/submodule.h"
#include "../include/refdb.h"
#include "../include/revwalk.h"
#include "../include/tag.h"
#include "../include/signature.h"
#include "../include/tree.h"
#include "../include/odb.h"
#include "../include/index.h"

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
  NODE_SET_PROTOTYPE_METHOD(tpl, "odb", Odb);
  NODE_SET_PROTOTYPE_METHOD(tpl, "openIndex", openIndex);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getBlob", GetBlob);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getCommit", GetCommit);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getObject", GetObject);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getReference", GetReference);
  NODE_SET_PROTOTYPE_METHOD(tpl, "createSymbolicReference", CreateSymbolicReference);
  NODE_SET_PROTOTYPE_METHOD(tpl, "createReference", CreateReference);
  NODE_SET_PROTOTYPE_METHOD(tpl, "createRevWalk", CreateRevWalk);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getSubmodule", GetSubmodule);
  NODE_SET_PROTOTYPE_METHOD(tpl, "addSubmodule", AddSubmodule);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getTag", GetTag);
  NODE_SET_PROTOTYPE_METHOD(tpl, "createTag", CreateTag);
  NODE_SET_METHOD(tpl, "createLightweightTag", CreateLightweightTag);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getTree", GetTree);
  NODE_SET_METHOD(tpl, "reloadSubmodules", ReloadSubmodules);
  NODE_SET_PROTOTYPE_METHOD(tpl, "delete", Delete);


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

Handle<Value> GitRepo::New(void *raw) {
  HandleScope scope;
  Handle<Value> argv[1] = { External::New((void *)raw) };
  return scope.Close(GitRepo::constructor_template->NewInstance(1, argv));
}

git_repository *GitRepo::GetValue() {
  return this->raw;
}


Handle<Value> GitRepo::Open(const Arguments& args) {
  HandleScope scope;
      if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String path is required.")));
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  OpenBaton* baton = new OpenBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->pathReference = Persistent<Value>::New(args[0]);
    String::Utf8Value path(args[0]->ToString());
  const char * from_path = strdup(*path);
  baton->path = from_path;
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
  baton->error_code = result;
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitRepo::OpenAfterWork(uv_work_t *req) {
  HandleScope scope;
  OpenBaton *baton = static_cast<OpenBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    to = GitRepo::New((void *)baton->out);
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
  baton->pathReference.Dispose();
  baton->callback.Dispose();
  free((void *)baton->path);
  delete baton;
}

Handle<Value> GitRepo::Init(const Arguments& args) {
  HandleScope scope;
      if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String path is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsBoolean()) {
    return ThrowException(Exception::Error(String::New("Boolean is_bare is required.")));
  }

  if (args.Length() == 2 || !args[2]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  InitBaton* baton = new InitBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->pathReference = Persistent<Value>::New(args[0]);
    String::Utf8Value path(args[0]->ToString());
  const char * from_path = strdup(*path);
  baton->path = from_path;
  baton->is_bareReference = Persistent<Value>::New(args[1]);
    unsigned from_is_bare = (unsigned) args[1]->ToBoolean()->Value();
  baton->is_bare = from_is_bare;
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
  baton->error_code = result;
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitRepo::InitAfterWork(uv_work_t *req) {
  HandleScope scope;
  InitBaton *baton = static_cast<InitBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    to = GitRepo::New((void *)baton->out);
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
  baton->pathReference.Dispose();
  baton->is_bareReference.Dispose();
  baton->callback.Dispose();
  free((void *)baton->path);
  delete baton;
}

Handle<Value> GitRepo::Path(const Arguments& args) {
  HandleScope scope;
  

  const char * result = git_repository_path(
    ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = String::New(result);
  return scope.Close(to);
}

Handle<Value> GitRepo::Workdir(const Arguments& args) {
  HandleScope scope;
  

  const char * result = git_repository_workdir(
    ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = String::New(result);
  return scope.Close(to);
}

Handle<Value> GitRepo::Odb(const Arguments& args) {
  HandleScope scope;
  
  git_odb *out = NULL;

  int result = git_repository_odb(
    &out
    , ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue()
  );
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  Handle<Value> to;
    to = GitOdb::New((void *)out);
  return scope.Close(to);
}

Handle<Value> GitRepo::openIndex(const Arguments& args) {
  HandleScope scope;
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  openIndexBaton* baton = new openIndexBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->repoReference = Persistent<Value>::New(args.This());
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, openIndexWork, (uv_after_work_cb)openIndexAfterWork);

  return Undefined();
}

void GitRepo::openIndexWork(uv_work_t *req) {
  openIndexBaton *baton = static_cast<openIndexBaton *>(req->data);
  int result = git_repository_index(
    &baton->out, 
    baton->repo
  );
  baton->error_code = result;
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitRepo::openIndexAfterWork(uv_work_t *req) {
  HandleScope scope;
  openIndexBaton *baton = static_cast<openIndexBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    to = GitIndex::New((void *)baton->out);
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
  baton->callback.Dispose();
  delete baton;
}

Handle<Value> GitRepo::GetBlob(const Arguments& args) {
  HandleScope scope;
      if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid id is required.")));
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  GetBlobBaton* baton = new GetBlobBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->repoReference = Persistent<Value>::New(args.This());
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue();
  baton->idReference = Persistent<Value>::New(args[0]);
    const git_oid * from_id = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject())->GetValue();
  baton->id = from_id;
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, GetBlobWork, (uv_after_work_cb)GetBlobAfterWork);

  return Undefined();
}

void GitRepo::GetBlobWork(uv_work_t *req) {
  GetBlobBaton *baton = static_cast<GetBlobBaton *>(req->data);
  int result = git_blob_lookup(
    &baton->blob, 
    baton->repo, 
    baton->id
  );
  baton->error_code = result;
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitRepo::GetBlobAfterWork(uv_work_t *req) {
  HandleScope scope;
  GetBlobBaton *baton = static_cast<GetBlobBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    to = GitBlob::New((void *)baton->blob);
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

Handle<Value> GitRepo::GetCommit(const Arguments& args) {
  HandleScope scope;
      if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid id is required.")));
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  GetCommitBaton* baton = new GetCommitBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->repoReference = Persistent<Value>::New(args.This());
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue();
  baton->idReference = Persistent<Value>::New(args[0]);
    const git_oid * from_id = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject())->GetValue();
  baton->id = from_id;
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, GetCommitWork, (uv_after_work_cb)GetCommitAfterWork);

  return Undefined();
}

void GitRepo::GetCommitWork(uv_work_t *req) {
  GetCommitBaton *baton = static_cast<GetCommitBaton *>(req->data);
  int result = git_commit_lookup(
    &baton->commit, 
    baton->repo, 
    baton->id
  );
  baton->error_code = result;
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitRepo::GetCommitAfterWork(uv_work_t *req) {
  HandleScope scope;
  GetCommitBaton *baton = static_cast<GetCommitBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    to = GitCommit::New((void *)baton->commit);
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

Handle<Value> GitRepo::GetObject(const Arguments& args) {
  HandleScope scope;
      if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid id is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number type is required.")));
  }

  if (args.Length() == 2 || !args[2]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  GetObjectBaton* baton = new GetObjectBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->repoReference = Persistent<Value>::New(args.This());
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue();
  baton->idReference = Persistent<Value>::New(args[0]);
    const git_oid * from_id = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject())->GetValue();
  baton->id = from_id;
  baton->typeReference = Persistent<Value>::New(args[1]);
    git_otype from_type = (git_otype) args[1]->ToInt32()->Value();
  baton->type = from_type;
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[2]));

  uv_queue_work(uv_default_loop(), &baton->request, GetObjectWork, (uv_after_work_cb)GetObjectAfterWork);

  return Undefined();
}

void GitRepo::GetObjectWork(uv_work_t *req) {
  GetObjectBaton *baton = static_cast<GetObjectBaton *>(req->data);
  int result = git_object_lookup(
    &baton->object, 
    baton->repo, 
    baton->id, 
    baton->type
  );
  baton->error_code = result;
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitRepo::GetObjectAfterWork(uv_work_t *req) {
  HandleScope scope;
  GetObjectBaton *baton = static_cast<GetObjectBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    to = GitObject::New((void *)baton->object);
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
  baton->typeReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

Handle<Value> GitRepo::GetReference(const Arguments& args) {
  HandleScope scope;
      if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String name is required.")));
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  GetReferenceBaton* baton = new GetReferenceBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->repoReference = Persistent<Value>::New(args.This());
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue();
  baton->nameReference = Persistent<Value>::New(args[0]);
    String::Utf8Value name(args[0]->ToString());
  const char * from_name = strdup(*name);
  baton->name = from_name;
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, GetReferenceWork, (uv_after_work_cb)GetReferenceAfterWork);

  return Undefined();
}

void GitRepo::GetReferenceWork(uv_work_t *req) {
  GetReferenceBaton *baton = static_cast<GetReferenceBaton *>(req->data);
  int result = git_reference_lookup(
    &baton->out, 
    baton->repo, 
    baton->name
  );
  baton->error_code = result;
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitRepo::GetReferenceAfterWork(uv_work_t *req) {
  HandleScope scope;
  GetReferenceBaton *baton = static_cast<GetReferenceBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    to = GitReference::New((void *)baton->out);
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
  baton->nameReference.Dispose();
  baton->callback.Dispose();
  free((void *)baton->name);
  delete baton;
}

Handle<Value> GitRepo::CreateSymbolicReference(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String name is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsString()) {
    return ThrowException(Exception::Error(String::New("String target is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number force is required.")));
  }

  git_reference *out = NULL;
  String::Utf8Value name(args[0]->ToString());
  const char * from_name = strdup(*name);
  String::Utf8Value target(args[1]->ToString());
  const char * from_target = strdup(*target);
  int from_force = (int) args[2]->ToInt32()->Value();

  int result = git_reference_symbolic_create(
    &out
    , ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue()
    , from_name
    , from_target
    , from_force
  );
  delete from_name;
  delete from_target;
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  Handle<Value> to;
    to = GitReference::New((void *)out);
  return scope.Close(to);
}

Handle<Value> GitRepo::CreateReference(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String name is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid id is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number force is required.")));
  }

  git_reference *out = NULL;
  String::Utf8Value name(args[0]->ToString());
  const char * from_name = strdup(*name);
  const git_oid * from_id = ObjectWrap::Unwrap<GitOid>(args[1]->ToObject())->GetValue();
  int from_force = (int) args[2]->ToInt32()->Value();

  int result = git_reference_create(
    &out
    , ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue()
    , from_name
    , from_id
    , from_force
  );
  delete from_name;
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  Handle<Value> to;
    to = GitReference::New((void *)out);
  return scope.Close(to);
}

Handle<Value> GitRepo::CreateRevWalk(const Arguments& args) {
  HandleScope scope;
  
  git_revwalk *out = NULL;

  int result = git_revwalk_new(
    &out
    , ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue()
  );
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  Handle<Value> to;
    to = GitRevWalk::New((void *)out);
  return scope.Close(to);
}

Handle<Value> GitRepo::GetSubmodule(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String name is required.")));
  }

  git_submodule *submodule = NULL;
  String::Utf8Value name(args[0]->ToString());
  const char * from_name = strdup(*name);

  int result = git_submodule_lookup(
    &submodule
    , ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue()
    , from_name
  );
  delete from_name;
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  Handle<Value> to;
    to = GitSubmodule::New((void *)submodule);
  return scope.Close(to);
}

Handle<Value> GitRepo::AddSubmodule(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String url is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsString()) {
    return ThrowException(Exception::Error(String::New("String path is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number use_gitlink is required.")));
  }

  git_submodule *submodule = NULL;
  String::Utf8Value url(args[0]->ToString());
  const char * from_url = strdup(*url);
  String::Utf8Value path(args[1]->ToString());
  const char * from_path = strdup(*path);
  int from_use_gitlink = (int) args[2]->ToInt32()->Value();

  int result = git_submodule_add_setup(
    &submodule
    , ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue()
    , from_url
    , from_path
    , from_use_gitlink
  );
  delete from_url;
  delete from_path;
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  Handle<Value> to;
    to = GitSubmodule::New((void *)submodule);
  return scope.Close(to);
}

Handle<Value> GitRepo::GetTag(const Arguments& args) {
  HandleScope scope;
      if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid id is required.")));
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  GetTagBaton* baton = new GetTagBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->repoReference = Persistent<Value>::New(args.This());
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue();
  baton->idReference = Persistent<Value>::New(args[0]);
    const git_oid * from_id = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject())->GetValue();
  baton->id = from_id;
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, GetTagWork, (uv_after_work_cb)GetTagAfterWork);

  return Undefined();
}

void GitRepo::GetTagWork(uv_work_t *req) {
  GetTagBaton *baton = static_cast<GetTagBaton *>(req->data);
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

void GitRepo::GetTagAfterWork(uv_work_t *req) {
  HandleScope scope;
  GetTagBaton *baton = static_cast<GetTagBaton *>(req->data);

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

Handle<Value> GitRepo::CreateTag(const Arguments& args) {
  HandleScope scope;
      if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String tag_name is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Object target is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Signature tagger is required.")));
  }
  if (args.Length() == 3 || !args[3]->IsString()) {
    return ThrowException(Exception::Error(String::New("String message is required.")));
  }
  if (args.Length() == 4 || !args[4]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number force is required.")));
  }

  if (args.Length() == 5 || !args[5]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  CreateTagBaton* baton = new CreateTagBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->oid = (git_oid *)malloc(sizeof(git_oid ));
  baton->repoReference = Persistent<Value>::New(args.This());
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue();
  baton->tag_nameReference = Persistent<Value>::New(args[0]);
    String::Utf8Value tag_name(args[0]->ToString());
  const char * from_tag_name = strdup(*tag_name);
  baton->tag_name = from_tag_name;
  baton->targetReference = Persistent<Value>::New(args[1]);
    const git_object * from_target = ObjectWrap::Unwrap<GitObject>(args[1]->ToObject())->GetValue();
  baton->target = from_target;
  baton->taggerReference = Persistent<Value>::New(args[2]);
    const git_signature * from_tagger = ObjectWrap::Unwrap<GitSignature>(args[2]->ToObject())->GetValue();
  baton->tagger = from_tagger;
  baton->messageReference = Persistent<Value>::New(args[3]);
    String::Utf8Value message(args[3]->ToString());
  const char * from_message = strdup(*message);
  baton->message = from_message;
  baton->forceReference = Persistent<Value>::New(args[4]);
    int from_force = (int) args[4]->ToInt32()->Value();
  baton->force = from_force;
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[5]));

  uv_queue_work(uv_default_loop(), &baton->request, CreateTagWork, (uv_after_work_cb)CreateTagAfterWork);

  return Undefined();
}

void GitRepo::CreateTagWork(uv_work_t *req) {
  CreateTagBaton *baton = static_cast<CreateTagBaton *>(req->data);
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

void GitRepo::CreateTagAfterWork(uv_work_t *req) {
  HandleScope scope;
  CreateTagBaton *baton = static_cast<CreateTagBaton *>(req->data);

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
  free((void *)baton->tag_name);
  free((void *)baton->message);
  delete baton;
}

Handle<Value> GitRepo::CreateLightweightTag(const Arguments& args) {
  HandleScope scope;
      if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String tag_name is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Object target is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number force is required.")));
  }

  if (args.Length() == 3 || !args[3]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  CreateLightweightTagBaton* baton = new CreateLightweightTagBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->oid = (git_oid *)malloc(sizeof(git_oid ));
  baton->repoReference = Persistent<Value>::New(args.This());
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue();
  baton->tag_nameReference = Persistent<Value>::New(args[0]);
    String::Utf8Value tag_name(args[0]->ToString());
  const char * from_tag_name = strdup(*tag_name);
  baton->tag_name = from_tag_name;
  baton->targetReference = Persistent<Value>::New(args[1]);
    const git_object * from_target = ObjectWrap::Unwrap<GitObject>(args[1]->ToObject())->GetValue();
  baton->target = from_target;
  baton->forceReference = Persistent<Value>::New(args[2]);
    int from_force = (int) args[2]->ToInt32()->Value();
  baton->force = from_force;
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[3]));

  uv_queue_work(uv_default_loop(), &baton->request, CreateLightweightTagWork, (uv_after_work_cb)CreateLightweightTagAfterWork);

  return Undefined();
}

void GitRepo::CreateLightweightTagWork(uv_work_t *req) {
  CreateLightweightTagBaton *baton = static_cast<CreateLightweightTagBaton *>(req->data);
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

void GitRepo::CreateLightweightTagAfterWork(uv_work_t *req) {
  HandleScope scope;
  CreateLightweightTagBaton *baton = static_cast<CreateLightweightTagBaton *>(req->data);

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
  free((void *)baton->tag_name);
  delete baton;
}

Handle<Value> GitRepo::GetTree(const Arguments& args) {
  HandleScope scope;
      if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid id is required.")));
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  GetTreeBaton* baton = new GetTreeBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->repoReference = Persistent<Value>::New(args.This());
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue();
  baton->idReference = Persistent<Value>::New(args[0]);
    const git_oid * from_id = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject())->GetValue();
  baton->id = from_id;
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, GetTreeWork, (uv_after_work_cb)GetTreeAfterWork);

  return Undefined();
}

void GitRepo::GetTreeWork(uv_work_t *req) {
  GetTreeBaton *baton = static_cast<GetTreeBaton *>(req->data);
  int result = git_tree_lookup(
    &baton->out, 
    baton->repo, 
    baton->id
  );
  baton->error_code = result;
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitRepo::GetTreeAfterWork(uv_work_t *req) {
  HandleScope scope;
  GetTreeBaton *baton = static_cast<GetTreeBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    to = GitTree::New((void *)baton->out);
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

Handle<Value> GitRepo::ReloadSubmodules(const Arguments& args) {
  HandleScope scope;
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  ReloadSubmodulesBaton* baton = new ReloadSubmodulesBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->repoReference = Persistent<Value>::New(args.This());
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, ReloadSubmodulesWork, (uv_after_work_cb)ReloadSubmodulesAfterWork);

  return Undefined();
}

void GitRepo::ReloadSubmodulesWork(uv_work_t *req) {
  ReloadSubmodulesBaton *baton = static_cast<ReloadSubmodulesBaton *>(req->data);
  int result = git_submodule_reload_all(
    baton->repo
  );
  baton->error_code = result;
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitRepo::ReloadSubmodulesAfterWork(uv_work_t *req) {
  HandleScope scope;
  ReloadSubmodulesBaton *baton = static_cast<ReloadSubmodulesBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {

    Handle<Value> result = Local<Value>::New(Undefined());
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
  baton->callback.Dispose();
  delete baton;
}

Handle<Value> GitRepo::Delete(const Arguments& args) {
  HandleScope scope;
      if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String tag_name is required.")));
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  DeleteBaton* baton = new DeleteBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->repoReference = Persistent<Value>::New(args.This());
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue();
  baton->tag_nameReference = Persistent<Value>::New(args[0]);
    String::Utf8Value tag_name(args[0]->ToString());
  const char * from_tag_name = strdup(*tag_name);
  baton->tag_name = from_tag_name;
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, DeleteWork, (uv_after_work_cb)DeleteAfterWork);

  return Undefined();
}

void GitRepo::DeleteWork(uv_work_t *req) {
  DeleteBaton *baton = static_cast<DeleteBaton *>(req->data);
  int result = git_tag_delete(
    baton->repo, 
    baton->tag_name
  );
  baton->error_code = result;
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitRepo::DeleteAfterWork(uv_work_t *req) {
  HandleScope scope;
  DeleteBaton *baton = static_cast<DeleteBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {

    Handle<Value> result = Local<Value>::New(Undefined());
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
  baton->callback.Dispose();
  free((void *)baton->tag_name);
  delete baton;
}

Persistent<Function> GitRepo::constructor_template;

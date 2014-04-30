/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/functions/copy.h"

#include "../include/submodule.h"
#include "../include/oid.h"
#include "../include/repo.h"

using namespace v8;
using namespace node;

GitSubmodule::GitSubmodule(git_submodule *raw) {
  this->raw = raw;
}

GitSubmodule::~GitSubmodule() {
  free(this->raw);
}

void GitSubmodule::Initialize(Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Submodule"));

  NODE_SET_PROTOTYPE_METHOD(tpl, "addFinalize", AddFinalize);
  NODE_SET_PROTOTYPE_METHOD(tpl, "addToIndex", AddToIndex);
  NODE_SET_PROTOTYPE_METHOD(tpl, "save", Save);
  NODE_SET_PROTOTYPE_METHOD(tpl, "name", Name);
  NODE_SET_PROTOTYPE_METHOD(tpl, "path", Path);
  NODE_SET_PROTOTYPE_METHOD(tpl, "url", Url);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setUrl", SetUrl);
  NODE_SET_PROTOTYPE_METHOD(tpl, "indexId", IndexId);
  NODE_SET_PROTOTYPE_METHOD(tpl, "headId", HeadId);
  NODE_SET_PROTOTYPE_METHOD(tpl, "init", Init);
  NODE_SET_PROTOTYPE_METHOD(tpl, "sync", Sync);
  NODE_SET_PROTOTYPE_METHOD(tpl, "open", Open);
  NODE_SET_PROTOTYPE_METHOD(tpl, "reload", Reload);
  NODE_SET_PROTOTYPE_METHOD(tpl, "status", Status);


  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Submodule"), constructor_template);
}

Handle<Value> GitSubmodule::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return ThrowException(Exception::Error(String::New("git_submodule is required.")));
  }

  GitSubmodule* object = new GitSubmodule((git_submodule *) External::Unwrap(args[0]));
  object->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> GitSubmodule::New(void *raw) {
  HandleScope scope;
  Handle<Value> argv[1] = { External::New((void *)raw) };
  return scope.Close(GitSubmodule::constructor_template->NewInstance(1, argv));
}

git_submodule *GitSubmodule::GetValue() {
  return this->raw;
}


#include "../include/functions/copy.h"

/**
 */
Handle<Value> GitSubmodule::AddFinalize(const Arguments& args) {
  HandleScope scope;
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  AddFinalizeBaton* baton = new AddFinalizeBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->submoduleReference = Persistent<Value>::New(args.This());
  baton->submodule = ObjectWrap::Unwrap<GitSubmodule>(args.This())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, AddFinalizeWork, (uv_after_work_cb)AddFinalizeAfterWork);

  return Undefined();
}

void GitSubmodule::AddFinalizeWork(uv_work_t *req) {
  AddFinalizeBaton *baton = static_cast<AddFinalizeBaton *>(req->data);
  int result = git_submodule_add_finalize(
    baton->submodule
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitSubmodule::AddFinalizeAfterWork(uv_work_t *req) {
  HandleScope scope;
  AddFinalizeBaton *baton = static_cast<AddFinalizeBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
    Handle<Value> result = Local<Value>::New(Undefined());
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
  baton->submoduleReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {Number} write_index
 */
Handle<Value> GitSubmodule::AddToIndex(const Arguments& args) {
  HandleScope scope;
      if (args.Length() == 0 || !args[0]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number write_index is required.")));
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  AddToIndexBaton* baton = new AddToIndexBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->submoduleReference = Persistent<Value>::New(args.This());
  baton->submodule = ObjectWrap::Unwrap<GitSubmodule>(args.This())->GetValue();
  baton->write_indexReference = Persistent<Value>::New(args[0]);
    int from_write_index;
            from_write_index = (int)   args[0]->ToInt32()->Value();
          baton->write_index = from_write_index;
    baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, AddToIndexWork, (uv_after_work_cb)AddToIndexAfterWork);

  return Undefined();
}

void GitSubmodule::AddToIndexWork(uv_work_t *req) {
  AddToIndexBaton *baton = static_cast<AddToIndexBaton *>(req->data);
  int result = git_submodule_add_to_index(
    baton->submodule, 
    baton->write_index
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitSubmodule::AddToIndexAfterWork(uv_work_t *req) {
  HandleScope scope;
  AddToIndexBaton *baton = static_cast<AddToIndexBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
    Handle<Value> result = Local<Value>::New(Undefined());
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
  baton->submoduleReference.Dispose();
  baton->write_indexReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

#include "../include/functions/copy.h"

/**
 */
Handle<Value> GitSubmodule::Save(const Arguments& args) {
  HandleScope scope;
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  SaveBaton* baton = new SaveBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->submoduleReference = Persistent<Value>::New(args.This());
  baton->submodule = ObjectWrap::Unwrap<GitSubmodule>(args.This())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, SaveWork, (uv_after_work_cb)SaveAfterWork);

  return Undefined();
}

void GitSubmodule::SaveWork(uv_work_t *req) {
  SaveBaton *baton = static_cast<SaveBaton *>(req->data);
  int result = git_submodule_save(
    baton->submodule
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitSubmodule::SaveAfterWork(uv_work_t *req) {
  HandleScope scope;
  SaveBaton *baton = static_cast<SaveBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
    Handle<Value> result = Local<Value>::New(Undefined());
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
  baton->submoduleReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

/**
 * @return {String} result
 */
Handle<Value> GitSubmodule::Name(const Arguments& args) {
  HandleScope scope;
  

  const char * result = git_submodule_name(
    ObjectWrap::Unwrap<GitSubmodule>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = String::New(result);
  return scope.Close(to);
}

/**
 * @return {String} result
 */
Handle<Value> GitSubmodule::Path(const Arguments& args) {
  HandleScope scope;
  

  const char * result = git_submodule_path(
    ObjectWrap::Unwrap<GitSubmodule>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = String::New(result);
  return scope.Close(to);
}

/**
 * @return {String} result
 */
Handle<Value> GitSubmodule::Url(const Arguments& args) {
  HandleScope scope;
  

  const char * result = git_submodule_url(
    ObjectWrap::Unwrap<GitSubmodule>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = String::New(result);
  return scope.Close(to);
}

/**
 * @param {String} url
 */
Handle<Value> GitSubmodule::SetUrl(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String url is required.")));
  }

  const char * from_url;
            String::Utf8Value url(args[0]->ToString());
      from_url = strdup(*url);
      
  int result = git_submodule_set_url(
    ObjectWrap::Unwrap<GitSubmodule>(args.This())->GetValue()
    , from_url
  );
  free((void *)from_url);
  if (result != GIT_OK) {
    if (giterr_last()) {
      return ThrowException(Exception::Error(String::New(giterr_last()->message)));
    } else {
      return ThrowException(Exception::Error(String::New("Unkown Error")));
    }
  }

  return Undefined();
}

/**
 * @return {Oid} result
 */
Handle<Value> GitSubmodule::IndexId(const Arguments& args) {
  HandleScope scope;
  

  const git_oid * result = git_submodule_index_id(
    ObjectWrap::Unwrap<GitSubmodule>(args.This())->GetValue()
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
 * @return {Oid} result
 */
Handle<Value> GitSubmodule::HeadId(const Arguments& args) {
  HandleScope scope;
  

  const git_oid * result = git_submodule_head_id(
    ObjectWrap::Unwrap<GitSubmodule>(args.This())->GetValue()
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
 * @param {Number} overwrite
 */
Handle<Value> GitSubmodule::Init(const Arguments& args) {
  HandleScope scope;
      if (args.Length() == 0 || !args[0]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number overwrite is required.")));
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  InitBaton* baton = new InitBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->submoduleReference = Persistent<Value>::New(args.This());
  baton->submodule = ObjectWrap::Unwrap<GitSubmodule>(args.This())->GetValue();
  baton->overwriteReference = Persistent<Value>::New(args[0]);
    int from_overwrite;
            from_overwrite = (int)   args[0]->ToInt32()->Value();
          baton->overwrite = from_overwrite;
    baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, InitWork, (uv_after_work_cb)InitAfterWork);

  return Undefined();
}

void GitSubmodule::InitWork(uv_work_t *req) {
  InitBaton *baton = static_cast<InitBaton *>(req->data);
  int result = git_submodule_init(
    baton->submodule, 
    baton->overwrite
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitSubmodule::InitAfterWork(uv_work_t *req) {
  HandleScope scope;
  InitBaton *baton = static_cast<InitBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
    Handle<Value> result = Local<Value>::New(Undefined());
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
  baton->submoduleReference.Dispose();
  baton->overwriteReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

#include "../include/functions/copy.h"

/**
 */
Handle<Value> GitSubmodule::Sync(const Arguments& args) {
  HandleScope scope;
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  SyncBaton* baton = new SyncBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->submoduleReference = Persistent<Value>::New(args.This());
  baton->submodule = ObjectWrap::Unwrap<GitSubmodule>(args.This())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, SyncWork, (uv_after_work_cb)SyncAfterWork);

  return Undefined();
}

void GitSubmodule::SyncWork(uv_work_t *req) {
  SyncBaton *baton = static_cast<SyncBaton *>(req->data);
  int result = git_submodule_sync(
    baton->submodule
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitSubmodule::SyncAfterWork(uv_work_t *req) {
  HandleScope scope;
  SyncBaton *baton = static_cast<SyncBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
    Handle<Value> result = Local<Value>::New(Undefined());
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
  baton->submoduleReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {Repository} callback
 */
Handle<Value> GitSubmodule::Open(const Arguments& args) {
  HandleScope scope;
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  OpenBaton* baton = new OpenBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->submoduleReference = Persistent<Value>::New(args.This());
  baton->submodule = ObjectWrap::Unwrap<GitSubmodule>(args.This())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, OpenWork, (uv_after_work_cb)OpenAfterWork);

  return Undefined();
}

void GitSubmodule::OpenWork(uv_work_t *req) {
  OpenBaton *baton = static_cast<OpenBaton *>(req->data);
  int result = git_submodule_open(
    &baton->repo, 
    baton->submodule
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitSubmodule::OpenAfterWork(uv_work_t *req) {
  HandleScope scope;
  OpenBaton *baton = static_cast<OpenBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->repo != NULL) {
    to = GitRepo::New((void *)baton->repo);
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
  baton->submoduleReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

#include "../include/functions/copy.h"

/**
 */
Handle<Value> GitSubmodule::Reload(const Arguments& args) {
  HandleScope scope;
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  ReloadBaton* baton = new ReloadBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->submoduleReference = Persistent<Value>::New(args.This());
  baton->submodule = ObjectWrap::Unwrap<GitSubmodule>(args.This())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, ReloadWork, (uv_after_work_cb)ReloadAfterWork);

  return Undefined();
}

void GitSubmodule::ReloadWork(uv_work_t *req) {
  ReloadBaton *baton = static_cast<ReloadBaton *>(req->data);
  int result = git_submodule_reload(
    baton->submodule
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitSubmodule::ReloadAfterWork(uv_work_t *req) {
  HandleScope scope;
  ReloadBaton *baton = static_cast<ReloadBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
    Handle<Value> result = Local<Value>::New(Undefined());
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
  baton->submoduleReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {Number} status
 */
Handle<Value> GitSubmodule::Status(const Arguments& args) {
  HandleScope scope;
      if (args.Length() == 0 || !args[0]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number status is required.")));
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  StatusBaton* baton = new StatusBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->statusReference = Persistent<Value>::New(args[0]);
    unsigned int * from_status;
            from_status = (unsigned int *)   args[0]->ToInt32()->Value();
          baton->status = from_status;
    baton->submoduleReference = Persistent<Value>::New(args.This());
  baton->submodule = ObjectWrap::Unwrap<GitSubmodule>(args.This())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, StatusWork, (uv_after_work_cb)StatusAfterWork);

  return Undefined();
}

void GitSubmodule::StatusWork(uv_work_t *req) {
  StatusBaton *baton = static_cast<StatusBaton *>(req->data);
  int result = git_submodule_status(
    baton->status, 
    baton->submodule
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitSubmodule::StatusAfterWork(uv_work_t *req) {
  HandleScope scope;
  StatusBaton *baton = static_cast<StatusBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
    Handle<Value> result = Local<Value>::New(Undefined());
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
  baton->statusReference.Dispose();
  baton->submoduleReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

Persistent<Function> GitSubmodule::constructor_template;

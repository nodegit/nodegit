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
  NanScope();

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanSymbol("Submodule"));

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


  NanAssignPersistent(FunctionTemplate, constructor_template, tpl);
  target->Set(String::NewSymbol("Submodule"), tpl->GetFunction());
}

NAN_METHOD(GitSubmodule::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError(String::New("git_submodule is required."));
  }

  GitSubmodule* object = new GitSubmodule((git_submodule *) External::Cast(*args[0])->Value());
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitSubmodule::New(void *raw) {
  NanScope();
  Handle<Value> argv[1] = { External::New((void *)raw) };
  Local<Object> instance;
  Local<FunctionTemplate> constructorHandle = NanPersistentToLocal(constructor_template);
  instance = constructorHandle->GetFunction()->NewInstance(1, argv);
  return scope.Close(instance);
}

git_submodule *GitSubmodule::GetValue() {
  return this->raw;
}


#include "../include/functions/copy.h"

/**
 */
NAN_METHOD(GitSubmodule::AddFinalize) {
  NanScope();
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return NanThrowError(String::New("Callback is required and must be a Function."));
  }

  AddFinalizeBaton* baton = new AddFinalizeBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  NanAssignPersistent(Value, baton->submoduleReference, args.This());
  baton->submodule = ObjectWrap::Unwrap<GitSubmodule>(args.This())->GetValue();
  NanAssignPersistent(Function, baton->callback, Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, AddFinalizeWork, (uv_after_work_cb)AddFinalizeAfterWork);

  NanReturnUndefined();
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
  NanScope();
  AddFinalizeBaton *baton = static_cast<AddFinalizeBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
    Handle<Value> result = NanNewLocal<Value>(Undefined());
    Handle<Value> argv[2] = {
      NanNewLocal<Value>(Null()),
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
  baton->submoduleReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {Number} write_index
 */
NAN_METHOD(GitSubmodule::AddToIndex) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsInt32()) {
    return NanThrowError(String::New("Number write_index is required."));
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError(String::New("Callback is required and must be a Function."));
  }

  AddToIndexBaton* baton = new AddToIndexBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  NanAssignPersistent(Value, baton->submoduleReference, args.This());
  baton->submodule = ObjectWrap::Unwrap<GitSubmodule>(args.This())->GetValue();
  NanAssignPersistent(Value, baton->write_indexReference, args[0]);
  int from_write_index;
  from_write_index = (int) args[0]->ToInt32()->Value();
  baton->write_index = from_write_index;
  NanAssignPersistent(Function, baton->callback, Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, AddToIndexWork, (uv_after_work_cb)AddToIndexAfterWork);

  NanReturnUndefined();
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
  NanScope();
  AddToIndexBaton *baton = static_cast<AddToIndexBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
    Handle<Value> result = NanNewLocal<Value>(Undefined());
    Handle<Value> argv[2] = {
      NanNewLocal<Value>(Null()),
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
  baton->submoduleReference.Dispose();
  baton->write_indexReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

#include "../include/functions/copy.h"

/**
 */
NAN_METHOD(GitSubmodule::Save) {
  NanScope();
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return NanThrowError(String::New("Callback is required and must be a Function."));
  }

  SaveBaton* baton = new SaveBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  NanAssignPersistent(Value, baton->submoduleReference, args.This());
  baton->submodule = ObjectWrap::Unwrap<GitSubmodule>(args.This())->GetValue();
  NanAssignPersistent(Function, baton->callback, Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, SaveWork, (uv_after_work_cb)SaveAfterWork);

  NanReturnUndefined();
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
  NanScope();
  SaveBaton *baton = static_cast<SaveBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
    Handle<Value> result = NanNewLocal<Value>(Undefined());
    Handle<Value> argv[2] = {
      NanNewLocal<Value>(Null()),
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
  baton->submoduleReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

/**
 * @return {String} result
 */
NAN_METHOD(GitSubmodule::Name) {
  NanScope();
  

  const char * result = git_submodule_name(
    ObjectWrap::Unwrap<GitSubmodule>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = String::New(result);
  NanReturnValue(to);
}

/**
 * @return {String} result
 */
NAN_METHOD(GitSubmodule::Path) {
  NanScope();
  

  const char * result = git_submodule_path(
    ObjectWrap::Unwrap<GitSubmodule>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = String::New(result);
  NanReturnValue(to);
}

/**
 * @return {String} result
 */
NAN_METHOD(GitSubmodule::Url) {
  NanScope();
  

  const char * result = git_submodule_url(
    ObjectWrap::Unwrap<GitSubmodule>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = String::New(result);
  NanReturnValue(to);
}

/**
 * @param {String} url
 */
NAN_METHOD(GitSubmodule::SetUrl) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError(String::New("String url is required."));
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
      return NanThrowError(String::New(giterr_last()->message));
    } else {
      return NanThrowError(String::New("Unkown Error"));
    }
  }

  NanReturnUndefined();
}

/**
 * @return {Oid} result
 */
NAN_METHOD(GitSubmodule::IndexId) {
  NanScope();
  

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
  NanReturnValue(to);
}

/**
 * @return {Oid} result
 */
NAN_METHOD(GitSubmodule::HeadId) {
  NanScope();
  

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
  NanReturnValue(to);
}

#include "../include/functions/copy.h"

/**
 * @param {Number} overwrite
 */
NAN_METHOD(GitSubmodule::Init) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsInt32()) {
    return NanThrowError(String::New("Number overwrite is required."));
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError(String::New("Callback is required and must be a Function."));
  }

  InitBaton* baton = new InitBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  NanAssignPersistent(Value, baton->submoduleReference, args.This());
  baton->submodule = ObjectWrap::Unwrap<GitSubmodule>(args.This())->GetValue();
  NanAssignPersistent(Value, baton->overwriteReference, args[0]);
  int from_overwrite;
  from_overwrite = (int) args[0]->ToInt32()->Value();
  baton->overwrite = from_overwrite;
  NanAssignPersistent(Function, baton->callback, Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, InitWork, (uv_after_work_cb)InitAfterWork);

  NanReturnUndefined();
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
  NanScope();
  InitBaton *baton = static_cast<InitBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
    Handle<Value> result = NanNewLocal<Value>(Undefined());
    Handle<Value> argv[2] = {
      NanNewLocal<Value>(Null()),
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
  baton->submoduleReference.Dispose();
  baton->overwriteReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

#include "../include/functions/copy.h"

/**
 */
NAN_METHOD(GitSubmodule::Sync) {
  NanScope();
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return NanThrowError(String::New("Callback is required and must be a Function."));
  }

  SyncBaton* baton = new SyncBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  NanAssignPersistent(Value, baton->submoduleReference, args.This());
  baton->submodule = ObjectWrap::Unwrap<GitSubmodule>(args.This())->GetValue();
  NanAssignPersistent(Function, baton->callback, Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, SyncWork, (uv_after_work_cb)SyncAfterWork);

  NanReturnUndefined();
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
  NanScope();
  SyncBaton *baton = static_cast<SyncBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
    Handle<Value> result = NanNewLocal<Value>(Undefined());
    Handle<Value> argv[2] = {
      NanNewLocal<Value>(Null()),
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
  baton->submoduleReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {Repository} callback
 */
NAN_METHOD(GitSubmodule::Open) {
  NanScope();
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return NanThrowError(String::New("Callback is required and must be a Function."));
  }

  OpenBaton* baton = new OpenBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  NanAssignPersistent(Value, baton->submoduleReference, args.This());
  baton->submodule = ObjectWrap::Unwrap<GitSubmodule>(args.This())->GetValue();
  NanAssignPersistent(Function, baton->callback, Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, OpenWork, (uv_after_work_cb)OpenAfterWork);

  NanReturnUndefined();
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
  NanScope();
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
      NanNewLocal<Value>(Null()),
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
  baton->submoduleReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

#include "../include/functions/copy.h"

/**
 */
NAN_METHOD(GitSubmodule::Reload) {
  NanScope();
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return NanThrowError(String::New("Callback is required and must be a Function."));
  }

  ReloadBaton* baton = new ReloadBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  NanAssignPersistent(Value, baton->submoduleReference, args.This());
  baton->submodule = ObjectWrap::Unwrap<GitSubmodule>(args.This())->GetValue();
  NanAssignPersistent(Function, baton->callback, Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, ReloadWork, (uv_after_work_cb)ReloadAfterWork);

  NanReturnUndefined();
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
  NanScope();
  ReloadBaton *baton = static_cast<ReloadBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
    Handle<Value> result = NanNewLocal<Value>(Undefined());
    Handle<Value> argv[2] = {
      NanNewLocal<Value>(Null()),
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
  baton->submoduleReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {Number} status
 */
NAN_METHOD(GitSubmodule::Status) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsInt32()) {
    return NanThrowError(String::New("Number status is required."));
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError(String::New("Callback is required and must be a Function."));
  }

  StatusBaton* baton = new StatusBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  NanAssignPersistent(Value, baton->statusReference, args[0]);
  unsigned int * from_status;
  from_status = (unsigned int *) args[0]->ToInt32()->Value();
  baton->status = from_status;
  NanAssignPersistent(Value, baton->submoduleReference, args.This());
  baton->submodule = ObjectWrap::Unwrap<GitSubmodule>(args.This())->GetValue();
  NanAssignPersistent(Function, baton->callback, Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, StatusWork, (uv_after_work_cb)StatusAfterWork);

  NanReturnUndefined();
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
  NanScope();
  StatusBaton *baton = static_cast<StatusBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
    Handle<Value> result = NanNewLocal<Value>(Undefined());
    Handle<Value> argv[2] = {
      NanNewLocal<Value>(Null()),
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
  baton->statusReference.Dispose();
  baton->submoduleReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

Persistent<FunctionTemplate> GitSubmodule::constructor_template;

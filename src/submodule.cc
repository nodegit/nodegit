/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <nan.h>
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

  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanNew<String>("Submodule"));

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


  Local<Function> _constructor_template = tpl->GetFunction();
  NanAssignPersistent(constructor_template, _constructor_template);
  target->Set(NanNew<String>("Submodule"), _constructor_template);
}

NAN_METHOD(GitSubmodule::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError("git_submodule is required.");
  }
  GitSubmodule* object = new GitSubmodule(static_cast<git_submodule *>(Handle<External>::Cast(args[0])->Value()));
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitSubmodule::New(void *raw) {
  NanEscapableScope();
  Handle<Value> argv[1] = { NanNew<External>((void *)raw) };
  return NanEscapeScope(NanNew<Function>(GitSubmodule::constructor_template)->NewInstance(1, argv));
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
    return NanThrowError("Callback is required and must be a Function.");
  }

  AddFinalizeBaton* baton = new AddFinalizeBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->submodule = ObjectWrap::Unwrap<GitSubmodule>(args.This())->GetValue();

  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[0]));
  AddFinalizeWorker *worker = new AddFinalizeWorker(baton, callback);
  worker->SaveToPersistent("submodule", args.This());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitSubmodule::AddFinalizeWorker::Execute() {
  int result = git_submodule_add_finalize(
    baton->submodule
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitSubmodule::AddFinalizeWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
    Handle<Value> result = NanUndefined();
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

#include "../include/functions/copy.h"

/**
 * @param {Number} write_index
 */
NAN_METHOD(GitSubmodule::AddToIndex) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsInt32()) {
    return NanThrowError("Number write_index is required.");
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  AddToIndexBaton* baton = new AddToIndexBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->submodule = ObjectWrap::Unwrap<GitSubmodule>(args.This())->GetValue();
    int from_write_index;
            from_write_index = (int)   args[0]->ToInt32()->Value();
          baton->write_index = from_write_index;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[1]));
  AddToIndexWorker *worker = new AddToIndexWorker(baton, callback);
  worker->SaveToPersistent("submodule", args.This());
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("write_index", args[0]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitSubmodule::AddToIndexWorker::Execute() {
  int result = git_submodule_add_to_index(
    baton->submodule, 
    baton->write_index
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitSubmodule::AddToIndexWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
    Handle<Value> result = NanUndefined();
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

#include "../include/functions/copy.h"

/**
 */
NAN_METHOD(GitSubmodule::Save) {
  NanScope();
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  SaveBaton* baton = new SaveBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->submodule = ObjectWrap::Unwrap<GitSubmodule>(args.This())->GetValue();

  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[0]));
  SaveWorker *worker = new SaveWorker(baton, callback);
  worker->SaveToPersistent("submodule", args.This());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitSubmodule::SaveWorker::Execute() {
  int result = git_submodule_save(
    baton->submodule
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitSubmodule::SaveWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
    Handle<Value> result = NanUndefined();
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
 * @return {String} result
 */
NAN_METHOD(GitSubmodule::Name) {
  NanScope();
  

  const char * result = git_submodule_name(
    ObjectWrap::Unwrap<GitSubmodule>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = NanNew<String>(result);
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
    to = NanNew<String>(result);
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
    to = NanNew<String>(result);
  NanReturnValue(to);
}

/**
 * @param {String} url
 */
NAN_METHOD(GitSubmodule::SetUrl) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError("String url is required.");
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
      return NanThrowError(giterr_last()->message);
    } else {
      return NanThrowError("Unknown Error");
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
    to = NanNull();
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
    to = NanNull();
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
    return NanThrowError("Number overwrite is required.");
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  InitBaton* baton = new InitBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->submodule = ObjectWrap::Unwrap<GitSubmodule>(args.This())->GetValue();
    int from_overwrite;
            from_overwrite = (int)   args[0]->ToInt32()->Value();
          baton->overwrite = from_overwrite;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[1]));
  InitWorker *worker = new InitWorker(baton, callback);
  worker->SaveToPersistent("submodule", args.This());
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("overwrite", args[0]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitSubmodule::InitWorker::Execute() {
  int result = git_submodule_init(
    baton->submodule, 
    baton->overwrite
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitSubmodule::InitWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
    Handle<Value> result = NanUndefined();
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

#include "../include/functions/copy.h"

/**
 */
NAN_METHOD(GitSubmodule::Sync) {
  NanScope();
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  SyncBaton* baton = new SyncBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->submodule = ObjectWrap::Unwrap<GitSubmodule>(args.This())->GetValue();

  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[0]));
  SyncWorker *worker = new SyncWorker(baton, callback);
  worker->SaveToPersistent("submodule", args.This());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitSubmodule::SyncWorker::Execute() {
  int result = git_submodule_sync(
    baton->submodule
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitSubmodule::SyncWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
    Handle<Value> result = NanUndefined();
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

#include "../include/functions/copy.h"

/**
 * @param {Repository} callback
 */
NAN_METHOD(GitSubmodule::Open) {
  NanScope();
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  OpenBaton* baton = new OpenBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->submodule = ObjectWrap::Unwrap<GitSubmodule>(args.This())->GetValue();

  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[0]));
  OpenWorker *worker = new OpenWorker(baton, callback);
  worker->SaveToPersistent("submodule", args.This());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitSubmodule::OpenWorker::Execute() {
  int result = git_submodule_open(
    &baton->repo, 
    baton->submodule
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitSubmodule::OpenWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->repo != NULL) {
    to = GitRepo::New((void *)baton->repo);
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

#include "../include/functions/copy.h"

/**
 */
NAN_METHOD(GitSubmodule::Reload) {
  NanScope();
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  ReloadBaton* baton = new ReloadBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->submodule = ObjectWrap::Unwrap<GitSubmodule>(args.This())->GetValue();

  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[0]));
  ReloadWorker *worker = new ReloadWorker(baton, callback);
  worker->SaveToPersistent("submodule", args.This());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitSubmodule::ReloadWorker::Execute() {
  int result = git_submodule_reload(
    baton->submodule
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitSubmodule::ReloadWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
    Handle<Value> result = NanUndefined();
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

#include "../include/functions/copy.h"

/**
 * @param {Number} status
 */
NAN_METHOD(GitSubmodule::Status) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsInt32()) {
    return NanThrowError("Number status is required.");
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  StatusBaton* baton = new StatusBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
    unsigned int * from_status;
            from_status = (unsigned int *)   args[0]->ToInt32()->Value();
          baton->status = from_status;
    baton->submodule = ObjectWrap::Unwrap<GitSubmodule>(args.This())->GetValue();

  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[1]));
  StatusWorker *worker = new StatusWorker(baton, callback);
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("status", args[0]->ToObject());
  worker->SaveToPersistent("submodule", args.This());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitSubmodule::StatusWorker::Execute() {
  int result = git_submodule_status(
    baton->status, 
    baton->submodule
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitSubmodule::StatusWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
    Handle<Value> result = NanUndefined();
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

Persistent<Function> GitSubmodule::constructor_template;

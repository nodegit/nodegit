/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <nan.h>
#include <string.h>

#include "git2.h"

#include "../include/functions/copy.h"

#include "../include/revwalk.h"
#include "../include/oid.h"
#include "../include/repo.h"

using namespace v8;
using namespace node;

GitRevWalk::GitRevWalk(git_revwalk *raw) {
  this->raw = raw;
}

GitRevWalk::~GitRevWalk() {
  git_revwalk_free(this->raw);
}

void GitRevWalk::Initialize(Handle<v8::Object> target) {
  NanScope();

  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanNew<String>("RevWalk"));

  NODE_SET_PROTOTYPE_METHOD(tpl, "reset", Reset);
  NODE_SET_PROTOTYPE_METHOD(tpl, "push", Push);
  NODE_SET_PROTOTYPE_METHOD(tpl, "pushGlob", PushGlob);
  NODE_SET_PROTOTYPE_METHOD(tpl, "pushHead", PushHead);
  NODE_SET_PROTOTYPE_METHOD(tpl, "hide", Hide);
  NODE_SET_PROTOTYPE_METHOD(tpl, "hideGlob", HideGlob);
  NODE_SET_PROTOTYPE_METHOD(tpl, "hideHead", HideHead);
  NODE_SET_PROTOTYPE_METHOD(tpl, "pushRef", PushRef);
  NODE_SET_PROTOTYPE_METHOD(tpl, "hideRef", HideRef);
  NODE_SET_PROTOTYPE_METHOD(tpl, "next", Next);
  NODE_SET_PROTOTYPE_METHOD(tpl, "sorting", Sorting);


  Local<Function> _constructor_template = tpl->GetFunction();
  NanAssignPersistent(constructor_template, _constructor_template);
  target->Set(NanNew<String>("RevWalk"), _constructor_template);
}

NAN_METHOD(GitRevWalk::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError("git_revwalk is required.");
  }
  GitRevWalk* object = new GitRevWalk(static_cast<git_revwalk *>(Handle<External>::Cast(args[0])->Value()));
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitRevWalk::New(void *raw) {
  NanEscapableScope();
  Handle<Value> argv[1] = { NanNew<External>((void *)raw) };
  return NanEscapeScope(NanNew<Function>(GitRevWalk::constructor_template)->NewInstance(1, argv));
}

git_revwalk *GitRevWalk::GetValue() {
  return this->raw;
}


/**
 */
NAN_METHOD(GitRevWalk::Reset) {
  NanScope();
  

  git_revwalk_reset(
    ObjectWrap::Unwrap<GitRevWalk>(args.This())->GetValue()
  );

  NanReturnUndefined();
}

#include "../include/functions/copy.h"

/**
 * @param {Oid} id
 */
NAN_METHOD(GitRevWalk::Push) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsObject()) {
    return NanThrowError("Oid id is required.");
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  PushBaton* baton = new PushBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->walk = ObjectWrap::Unwrap<GitRevWalk>(args.This())->GetValue();
    const git_oid * from_id;
            from_id = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject())->GetValue();
          baton->id = from_id;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[1]));
  PushWorker *worker = new PushWorker(baton, callback);
  worker->SaveToPersistent("walk", args.This());
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("id", args[0]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitRevWalk::PushWorker::Execute() {
  int result = git_revwalk_push(
    baton->walk, 
    baton->id
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRevWalk::PushWorker::HandleOKCallback() {
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
 * @param {String} glob
 */
NAN_METHOD(GitRevWalk::PushGlob) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError("String glob is required.");
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  PushGlobBaton* baton = new PushGlobBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->walk = ObjectWrap::Unwrap<GitRevWalk>(args.This())->GetValue();
    const char * from_glob;
            String::Utf8Value glob(args[0]->ToString());
      from_glob = strdup(*glob);
          baton->glob = from_glob;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[1]));
  PushGlobWorker *worker = new PushGlobWorker(baton, callback);
  worker->SaveToPersistent("walk", args.This());
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("glob", args[0]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitRevWalk::PushGlobWorker::Execute() {
  int result = git_revwalk_push_glob(
    baton->walk, 
    baton->glob
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRevWalk::PushGlobWorker::HandleOKCallback() {
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
  free((void *)baton->glob);
  delete baton;
}

#include "../include/functions/copy.h"

/**
 */
NAN_METHOD(GitRevWalk::PushHead) {
  NanScope();
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  PushHeadBaton* baton = new PushHeadBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->walk = ObjectWrap::Unwrap<GitRevWalk>(args.This())->GetValue();

  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[0]));
  PushHeadWorker *worker = new PushHeadWorker(baton, callback);
  worker->SaveToPersistent("walk", args.This());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitRevWalk::PushHeadWorker::Execute() {
  int result = git_revwalk_push_head(
    baton->walk
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRevWalk::PushHeadWorker::HandleOKCallback() {
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
 * @param {Oid} commit_id
 */
NAN_METHOD(GitRevWalk::Hide) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsObject()) {
    return NanThrowError("Oid commit_id is required.");
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  HideBaton* baton = new HideBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->walk = ObjectWrap::Unwrap<GitRevWalk>(args.This())->GetValue();
    const git_oid * from_commit_id;
            from_commit_id = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject())->GetValue();
          baton->commit_id = from_commit_id;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[1]));
  HideWorker *worker = new HideWorker(baton, callback);
  worker->SaveToPersistent("walk", args.This());
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("commit_id", args[0]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitRevWalk::HideWorker::Execute() {
  int result = git_revwalk_hide(
    baton->walk, 
    baton->commit_id
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRevWalk::HideWorker::HandleOKCallback() {
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
 * @param {String} glob
 */
NAN_METHOD(GitRevWalk::HideGlob) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError("String glob is required.");
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  HideGlobBaton* baton = new HideGlobBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->walk = ObjectWrap::Unwrap<GitRevWalk>(args.This())->GetValue();
    const char * from_glob;
            String::Utf8Value glob(args[0]->ToString());
      from_glob = strdup(*glob);
          baton->glob = from_glob;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[1]));
  HideGlobWorker *worker = new HideGlobWorker(baton, callback);
  worker->SaveToPersistent("walk", args.This());
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("glob", args[0]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitRevWalk::HideGlobWorker::Execute() {
  int result = git_revwalk_hide_glob(
    baton->walk, 
    baton->glob
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRevWalk::HideGlobWorker::HandleOKCallback() {
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
  free((void *)baton->glob);
  delete baton;
}

#include "../include/functions/copy.h"

/**
 */
NAN_METHOD(GitRevWalk::HideHead) {
  NanScope();
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  HideHeadBaton* baton = new HideHeadBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->walk = ObjectWrap::Unwrap<GitRevWalk>(args.This())->GetValue();

  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[0]));
  HideHeadWorker *worker = new HideHeadWorker(baton, callback);
  worker->SaveToPersistent("walk", args.This());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitRevWalk::HideHeadWorker::Execute() {
  int result = git_revwalk_hide_head(
    baton->walk
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRevWalk::HideHeadWorker::HandleOKCallback() {
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
 * @param {String} refname
 */
NAN_METHOD(GitRevWalk::PushRef) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError("String refname is required.");
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  PushRefBaton* baton = new PushRefBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->walk = ObjectWrap::Unwrap<GitRevWalk>(args.This())->GetValue();
    const char * from_refname;
            String::Utf8Value refname(args[0]->ToString());
      from_refname = strdup(*refname);
          baton->refname = from_refname;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[1]));
  PushRefWorker *worker = new PushRefWorker(baton, callback);
  worker->SaveToPersistent("walk", args.This());
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("refname", args[0]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitRevWalk::PushRefWorker::Execute() {
  int result = git_revwalk_push_ref(
    baton->walk, 
    baton->refname
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRevWalk::PushRefWorker::HandleOKCallback() {
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
  free((void *)baton->refname);
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {String} refname
 */
NAN_METHOD(GitRevWalk::HideRef) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError("String refname is required.");
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  HideRefBaton* baton = new HideRefBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->walk = ObjectWrap::Unwrap<GitRevWalk>(args.This())->GetValue();
    const char * from_refname;
            String::Utf8Value refname(args[0]->ToString());
      from_refname = strdup(*refname);
          baton->refname = from_refname;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[1]));
  HideRefWorker *worker = new HideRefWorker(baton, callback);
  worker->SaveToPersistent("walk", args.This());
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("refname", args[0]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitRevWalk::HideRefWorker::Execute() {
  int result = git_revwalk_hide_ref(
    baton->walk, 
    baton->refname
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRevWalk::HideRefWorker::HandleOKCallback() {
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
  free((void *)baton->refname);
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {Oid} callback
 */
NAN_METHOD(GitRevWalk::Next) {
  NanScope();
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  NextBaton* baton = new NextBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->out = (git_oid *)malloc(sizeof(git_oid ));
  baton->walk = ObjectWrap::Unwrap<GitRevWalk>(args.This())->GetValue();

  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[0]));
  NextWorker *worker = new NextWorker(baton, callback);
  worker->SaveToPersistent("walk", args.This());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitRevWalk::NextWorker::Execute() {
  int result = git_revwalk_next(
    baton->out, 
    baton->walk
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRevWalk::NextWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->out != NULL) {
    to = GitOid::New((void *)baton->out);
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
        free(baton->out);
      }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  delete baton;
}

/**
 * @param {Number} sort_mode
 */
NAN_METHOD(GitRevWalk::Sorting) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsUint32()) {
    return NanThrowError("Number sort_mode is required.");
  }

  unsigned int from_sort_mode;
            from_sort_mode = (unsigned int)   args[0]->ToUint32()->Value();
      
  git_revwalk_sorting(
    ObjectWrap::Unwrap<GitRevWalk>(args.This())->GetValue()
    , from_sort_mode
  );

  NanReturnUndefined();
}

Persistent<Function> GitRevWalk::constructor_template;

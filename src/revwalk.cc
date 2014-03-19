/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
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

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanSymbol("RevWalk"));

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


  NanAssignPersistent(FunctionTemplate, constructor_template, tpl);
  target->Set(String::NewSymbol("RevWalk"), tpl->GetFunction());
}
NAN_METHOD(GitRevWalk::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError(String::New("git_revwalk is required."));
  }

  GitRevWalk* object = new GitRevWalk((git_revwalk *) External::Cast(*args[0])->Value());
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitRevWalk::New(void *raw) {
  NanScope();
  Handle<Value> argv[1] = { External::New((void *)raw) };
  Local<Object> instance;
  Local<FunctionTemplate> constructorHandle = NanPersistentToLocal(constructor_template);
  instance = constructorHandle->GetFunction()->NewInstance(1, argv);
  return scope.Close(instance);
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
    return NanThrowError(String::New("Oid id is required."));
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError(String::New("Callback is required and must be a Function."));
  }

  PushBaton* baton = new PushBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  NanAssignPersistent(Value, baton->walkReference, args.This());
  baton->walk = ObjectWrap::Unwrap<GitRevWalk>(args.This())->GetValue();
  NanAssignPersistent(Value, baton->idReference, args[0]);
  const git_oid * from_id;
  from_id = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject())->GetValue();
  baton->id = from_id;
  NanAssignPersistent(Function, baton->callback, Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, PushWork, (uv_after_work_cb)PushAfterWork);

  NanReturnUndefined();
}

void GitRevWalk::PushWork(uv_work_t *req) {
  PushBaton *baton = static_cast<PushBaton *>(req->data);
  int result = git_revwalk_push(
    baton->walk, 
    baton->id
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRevWalk::PushAfterWork(uv_work_t *req) {
  NanScope();
  PushBaton *baton = static_cast<PushBaton *>(req->data);

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
  baton->walkReference.Dispose();
  baton->idReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {String} glob
 */
NAN_METHOD(GitRevWalk::PushGlob) {
  NanScope();
  if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError(String::New("String glob is required."));
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError(String::New("Callback is required and must be a Function."));
  }

  PushGlobBaton* baton = new PushGlobBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  NanAssignPersistent(Value, baton->walkReference, args.This());
  baton->walk = ObjectWrap::Unwrap<GitRevWalk>(args.This())->GetValue();
  NanAssignPersistent(Value, baton->globReference, args[0]);
  const char * from_glob;
  String::Utf8Value glob(args[0]->ToString());
  from_glob = strdup(*glob);
  baton->glob = from_glob;
  NanAssignPersistent(Function, baton->callback, Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, PushGlobWork, (uv_after_work_cb)PushGlobAfterWork);

  NanReturnUndefined();
}

void GitRevWalk::PushGlobWork(uv_work_t *req) {
  PushGlobBaton *baton = static_cast<PushGlobBaton *>(req->data);
  int result = git_revwalk_push_glob(
    baton->walk, 
    baton->glob
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRevWalk::PushGlobAfterWork(uv_work_t *req) {
  NanScope();
  PushGlobBaton *baton = static_cast<PushGlobBaton *>(req->data);

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
  baton->walkReference.Dispose();
  baton->globReference.Dispose();
  baton->callback.Dispose();
  free((void *)baton->glob);
  delete baton;
}

#include "../include/functions/copy.h"

/**
 */
NAN_METHOD(GitRevWalk::PushHead) {
  NanScope();
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return NanThrowError(String::New("Callback is required and must be a Function."));
  }

  PushHeadBaton* baton = new PushHeadBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  NanAssignPersistent(Value, baton->walkReference, args.This());
  baton->walk = ObjectWrap::Unwrap<GitRevWalk>(args.This())->GetValue();
  NanAssignPersistent(Function, baton->callback, Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, PushHeadWork, (uv_after_work_cb)PushHeadAfterWork);

  NanReturnUndefined();
}

void GitRevWalk::PushHeadWork(uv_work_t *req) {
  PushHeadBaton *baton = static_cast<PushHeadBaton *>(req->data);
  int result = git_revwalk_push_head(
    baton->walk
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRevWalk::PushHeadAfterWork(uv_work_t *req) {
  NanScope();
  PushHeadBaton *baton = static_cast<PushHeadBaton *>(req->data);

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
  baton->walkReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {Oid} commit_id
 */
NAN_METHOD(GitRevWalk::Hide) {
  NanScope();
  if (args.Length() == 0 || !args[0]->IsObject()) {
    return NanThrowError(String::New("Oid commit_id is required."));
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError(String::New("Callback is required and must be a Function."));
  }

  HideBaton* baton = new HideBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  NanAssignPersistent(Value, baton->walkReference, args.This());
  baton->walk = ObjectWrap::Unwrap<GitRevWalk>(args.This())->GetValue();
  NanAssignPersistent(Value, baton->commit_idReference, args[0]);
  const git_oid * from_commit_id;
  from_commit_id = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject())->GetValue();
  baton->commit_id = from_commit_id;
  NanAssignPersistent(Function, baton->callback, Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, HideWork, (uv_after_work_cb)HideAfterWork);

  NanReturnUndefined();
}

void GitRevWalk::HideWork(uv_work_t *req) {
  HideBaton *baton = static_cast<HideBaton *>(req->data);
  int result = git_revwalk_hide(
    baton->walk, 
    baton->commit_id
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRevWalk::HideAfterWork(uv_work_t *req) {
  NanScope();
  HideBaton *baton = static_cast<HideBaton *>(req->data);

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
  baton->walkReference.Dispose();
  baton->commit_idReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {String} glob
 */
NAN_METHOD(GitRevWalk::HideGlob) {
  NanScope();
  if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError(String::New("String glob is required."));
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError(String::New("Callback is required and must be a Function."));
  }

  HideGlobBaton* baton = new HideGlobBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  NanAssignPersistent(Value, baton->walkReference, args.This());
  baton->walk = ObjectWrap::Unwrap<GitRevWalk>(args.This())->GetValue();
  NanAssignPersistent(Value, baton->globReference, args[0]);
  const char * from_glob;
  String::Utf8Value glob(args[0]->ToString());
  from_glob = strdup(*glob);
  baton->glob = from_glob;
  NanAssignPersistent(Function, baton->callback, Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, HideGlobWork, (uv_after_work_cb)HideGlobAfterWork);

  NanReturnUndefined();
}

void GitRevWalk::HideGlobWork(uv_work_t *req) {
  HideGlobBaton *baton = static_cast<HideGlobBaton *>(req->data);
  int result = git_revwalk_hide_glob(
    baton->walk, 
    baton->glob
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRevWalk::HideGlobAfterWork(uv_work_t *req) {
  NanScope();
  HideGlobBaton *baton = static_cast<HideGlobBaton *>(req->data);

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
  baton->walkReference.Dispose();
  baton->globReference.Dispose();
  baton->callback.Dispose();
  free((void *)baton->glob);
  delete baton;
}

#include "../include/functions/copy.h"

/**
 */
NAN_METHOD(GitRevWalk::HideHead) {
  NanScope();
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return NanThrowError(String::New("Callback is required and must be a Function."));
  }

  HideHeadBaton* baton = new HideHeadBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  NanAssignPersistent(Value, baton->walkReference, args.This());
  baton->walk = ObjectWrap::Unwrap<GitRevWalk>(args.This())->GetValue();
  NanAssignPersistent(Function, baton->callback, Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, HideHeadWork, (uv_after_work_cb)HideHeadAfterWork);

  NanReturnUndefined();
}

void GitRevWalk::HideHeadWork(uv_work_t *req) {
  HideHeadBaton *baton = static_cast<HideHeadBaton *>(req->data);
  int result = git_revwalk_hide_head(
    baton->walk
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRevWalk::HideHeadAfterWork(uv_work_t *req) {
  NanScope();
  HideHeadBaton *baton = static_cast<HideHeadBaton *>(req->data);

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
  baton->walkReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {String} refname
 */
NAN_METHOD(GitRevWalk::PushRef) {
  NanScope();
  if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError(String::New("String refname is required."));
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError(String::New("Callback is required and must be a Function."));
  }

  PushRefBaton* baton = new PushRefBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  NanAssignPersistent(Value, baton->walkReference, args.This());
  baton->walk = ObjectWrap::Unwrap<GitRevWalk>(args.This())->GetValue();
  NanAssignPersistent(Value, baton->refnameReference, args[0]);
  const char * from_refname;
  String::Utf8Value refname(args[0]->ToString());
  from_refname = strdup(*refname);
  baton->refname = from_refname;
  NanAssignPersistent(Function, baton->callback, Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, PushRefWork, (uv_after_work_cb)PushRefAfterWork);

  NanReturnUndefined();
}

void GitRevWalk::PushRefWork(uv_work_t *req) {
  PushRefBaton *baton = static_cast<PushRefBaton *>(req->data);
  int result = git_revwalk_push_ref(
    baton->walk, 
    baton->refname
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRevWalk::PushRefAfterWork(uv_work_t *req) {
  NanScope();
  PushRefBaton *baton = static_cast<PushRefBaton *>(req->data);

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
  baton->walkReference.Dispose();
  baton->refnameReference.Dispose();
  baton->callback.Dispose();
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
    return NanThrowError(String::New("String refname is required."));
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError(String::New("Callback is required and must be a Function."));
  }

  HideRefBaton* baton = new HideRefBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  NanAssignPersistent(Value, baton->walkReference, args.This());
  baton->walk = ObjectWrap::Unwrap<GitRevWalk>(args.This())->GetValue();
  NanAssignPersistent(Value, baton->refnameReference, args[0]);
  const char * from_refname;
  String::Utf8Value refname(args[0]->ToString());
  from_refname = strdup(*refname);
  baton->refname = from_refname;
  NanAssignPersistent(Function, baton->callback, Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, HideRefWork, (uv_after_work_cb)HideRefAfterWork);

  NanReturnUndefined();
}

void GitRevWalk::HideRefWork(uv_work_t *req) {
  HideRefBaton *baton = static_cast<HideRefBaton *>(req->data);
  int result = git_revwalk_hide_ref(
    baton->walk, 
    baton->refname
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRevWalk::HideRefAfterWork(uv_work_t *req) {
  NanScope();
  HideRefBaton *baton = static_cast<HideRefBaton *>(req->data);

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
  baton->walkReference.Dispose();
  baton->refnameReference.Dispose();
  baton->callback.Dispose();
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
    return NanThrowError(String::New("Callback is required and must be a Function."));
  }

  NextBaton* baton = new NextBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->out = (git_oid *)malloc(sizeof(git_oid ));
  NanAssignPersistent(Value, baton->walkReference, args.This());
  baton->walk = ObjectWrap::Unwrap<GitRevWalk>(args.This())->GetValue();
  NanAssignPersistent(Function, baton->callback, Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, NextWork, (uv_after_work_cb)NextAfterWork);

  NanReturnUndefined();
}

void GitRevWalk::NextWork(uv_work_t *req) {
  NextBaton *baton = static_cast<NextBaton *>(req->data);
  int result = git_revwalk_next(
    baton->out, 
    baton->walk
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRevWalk::NextAfterWork(uv_work_t *req) {
  NanScope();
  NextBaton *baton = static_cast<NextBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->out != NULL) {
    to = GitOid::New((void *)baton->out);
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
        free(baton->out);
      }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  baton->walkReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

/**
 * @param {Number} sort_mode
 */
NAN_METHOD(GitRevWalk::Sorting) {
  NanScope();
  if (args.Length() == 0 || !args[0]->IsUint32()) {
    return NanThrowError(String::New("Number sort_mode is required."));
  }

  unsigned int from_sort_mode;
            from_sort_mode = (unsigned int) args[0]->ToUint32()->Value();
      
  git_revwalk_sorting(
    ObjectWrap::Unwrap<GitRevWalk>(args.This())->GetValue()
    , from_sort_mode
  );

  NanReturnUndefined();
}

Persistent<FunctionTemplate> GitRevWalk::constructor_template;

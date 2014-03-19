/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/functions/copy.h"

#include "../include/reference.h"
#include "../include/repo.h"
#include "../include/oid.h"
#include "../include/object.h"

using namespace v8;
using namespace node;

GitReference::GitReference(git_reference *raw) {
  this->raw = raw;
}

GitReference::~GitReference() {
  git_reference_free(this->raw);
}

void GitReference::Initialize(Handle<v8::Object> target) {
  NanScope();

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanSymbol("Reference"));

  NODE_SET_METHOD(tpl, "oidForName", OidForName);
  NODE_SET_PROTOTYPE_METHOD(tpl, "target", Target);
  NODE_SET_PROTOTYPE_METHOD(tpl, "symbolicTarget", SymbolicTarget);
  NODE_SET_PROTOTYPE_METHOD(tpl, "type", Type);
  NODE_SET_PROTOTYPE_METHOD(tpl, "name", Name);
  NODE_SET_PROTOTYPE_METHOD(tpl, "resolve", Resolve);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setSymbolicTarget", SetSymbolicTarget);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setTarget", setTarget);
  NODE_SET_PROTOTYPE_METHOD(tpl, "rename", Rename);
  NODE_SET_PROTOTYPE_METHOD(tpl, "delete", Delete);
  NODE_SET_PROTOTYPE_METHOD(tpl, "isBranch", IsBranch);
  NODE_SET_PROTOTYPE_METHOD(tpl, "isRemote", IsRemote);
  NODE_SET_PROTOTYPE_METHOD(tpl, "peel", Peel);
  NODE_SET_METHOD(tpl, "isValidName", IsValidName);

  NanAssignPersistent(FunctionTemplate, constructor_template, tpl);
  target->Set(String::NewSymbol("Reference"), tpl->GetFunction());
}

NAN_METHOD(GitReference::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError(String::New("git_reference is required."));
  }

  GitReference* object = new GitReference((git_reference *) External::Cast(*args[0])->Value());
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitReference::New(void *raw) {
  NanScope();
  Handle<Value> argv[1] = { External::New((void *)raw) };
  Local<Object> instance;
  Local<FunctionTemplate> constructorHandle = NanPersistentToLocal(constructor_template);
  instance = constructorHandle->GetFunction()->NewInstance(1, argv);
  return scope.Close(instance);
}

git_reference *GitReference::GetValue() {
  return this->raw;
}


#include "../include/functions/copy.h"

/**
 * @param {Repository} repo
 * @param {String} name
 * @param {Oid} callback
 */
NAN_METHOD(GitReference::OidForName) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsObject()) {
    return NanThrowError(String::New("Repository repo is required."));
  }
  if (args.Length() == 1 || !args[1]->IsString()) {
    return NanThrowError(String::New("String name is required."));
  }

  if (args.Length() == 2 || !args[2]->IsFunction()) {
    return NanThrowError(String::New("Callback is required and must be a Function."));
  }

  OidForNameBaton* baton = new OidForNameBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->out = (git_oid *)malloc(sizeof(git_oid ));

  NanAssignPersistent(Value, baton->repoReference, args[0]);
  git_repository * from_repo;
  from_repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
  baton->repo = from_repo;
  NanAssignPersistent(Value, baton->nameReference, args[1]);
  const char * from_name;
  String::Utf8Value name(args[1]->ToString());
  from_name = strdup(*name);
  baton->name = from_name;
  NanAssignPersistent(Function, baton->callback, Local<Function>::Cast(args[2]));

  uv_queue_work(uv_default_loop(), &baton->request, OidForNameWork, (uv_after_work_cb)OidForNameAfterWork);

  NanReturnUndefined();
}

void GitReference::OidForNameWork(uv_work_t *req) {
  OidForNameBaton *baton = static_cast<OidForNameBaton *>(req->data);
  int result = git_reference_name_to_id(
    baton->out, 
    baton->repo, 
    baton->name
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitReference::OidForNameAfterWork(uv_work_t *req) {
  NanScope();
  OidForNameBaton *baton = static_cast<OidForNameBaton *>(req->data);

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
  baton->repoReference.Dispose();
  baton->nameReference.Dispose();
  baton->callback.Dispose();
  free((void *)baton->name);
  delete baton;
}

/**
 * @return {Oid} result
 */
NAN_METHOD(GitReference::Target) {
  NanScope();


  const git_oid * result = git_reference_target(
    ObjectWrap::Unwrap<GitReference>(args.This())->GetValue()
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
 * @return {String} result
 */
NAN_METHOD(GitReference::SymbolicTarget) {
  NanScope();
  

  const char * result = git_reference_symbolic_target(
    ObjectWrap::Unwrap<GitReference>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = String::New(result);
  NanReturnValue(to);
}

/**
 * @return {Number} result
 */
NAN_METHOD(GitReference::Type) {
  NanScope();


  git_ref_t result = git_reference_type(
    ObjectWrap::Unwrap<GitReference>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = Number::New(result);
  NanReturnValue(to);
}

/**
 * @return {String} result
 */
NAN_METHOD(GitReference::Name) {
  NanScope();


  const char * result = git_reference_name(
    ObjectWrap::Unwrap<GitReference>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = String::New(result);
  NanReturnValue(to);
}

#include "../include/functions/copy.h"

/**
 * @param {Reference} callback
 */
NAN_METHOD(GitReference::Resolve) {
  NanScope();

    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return NanThrowError(String::New("Callback is required and must be a Function."));
  }

  ResolveBaton* baton = new ResolveBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  NanAssignPersistent(Value, baton->refReference, args.This());
  baton->ref = ObjectWrap::Unwrap<GitReference>(args.This())->GetValue();
  NanAssignPersistent(Function, baton->callback, Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, ResolveWork, (uv_after_work_cb)ResolveAfterWork);

  NanReturnUndefined();
}

void GitReference::ResolveWork(uv_work_t *req) {
  ResolveBaton *baton = static_cast<ResolveBaton *>(req->data);
  int result = git_reference_resolve(
    &baton->out, 
    baton->ref
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitReference::ResolveAfterWork(uv_work_t *req) {
  NanScope();
  ResolveBaton *baton = static_cast<ResolveBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->out != NULL) {
    to = GitReference::New((void *)baton->out);
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
  baton->refReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

/**
 * @param {String} target
 * @return {Reference} out
 */
NAN_METHOD(GitReference::SetSymbolicTarget) {
  NanScope();
  if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError(String::New("String target is required."));
  }

  git_reference * out = 0;
  const char * from_target;
            String::Utf8Value target(args[0]->ToString());
      from_target = strdup(*target);
      
  int result = git_reference_symbolic_set_target(
    &out
    , ObjectWrap::Unwrap<GitReference>(args.This())->GetValue()
    , from_target
  );
  free((void *)from_target);
  if (result != GIT_OK) {
    if (giterr_last()) {
      return NanThrowError(String::New(giterr_last()->message));
    } else {
      return NanThrowError(String::New("Unkown Error"));
    }
  }

  Handle<Value> to;
    if (out != NULL) {
    to = GitReference::New((void *)out);
  } else {
    to = Null();
  }
  NanReturnValue(to);
}

/**
 * @param {Oid} id
 * @return {Reference} out
 */
NAN_METHOD(GitReference::setTarget) {
  NanScope();
  if (args.Length() == 0 || !args[0]->IsObject()) {
    return NanThrowError(String::New("Oid id is required."));
  }

  git_reference * out = 0;
  const git_oid * from_id;
            from_id = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject())->GetValue();
      
  int result = git_reference_set_target(
    &out
    , ObjectWrap::Unwrap<GitReference>(args.This())->GetValue()
    , from_id
  );
  if (result != GIT_OK) {
    if (giterr_last()) {
      return NanThrowError(String::New(giterr_last()->message));
    } else {
      return NanThrowError(String::New("Unkown Error"));
    }
  }

  Handle<Value> to;
    if (out != NULL) {
    to = GitReference::New((void *)out);
  } else {
    to = Null();
  }
  NanReturnValue(to);
}

#include "../include/functions/copy.h"

/**
 * @param {String} new_name
 * @param {Number} force
 * @param {Reference} callback
 */
NAN_METHOD(GitReference::Rename) {
  NanScope();
  if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError(String::New("String new_name is required."));
  }
  if (args.Length() == 1 || !args[1]->IsInt32()) {
    return NanThrowError(String::New("Number force is required."));
  }

  if (args.Length() == 2 || !args[2]->IsFunction()) {
    return NanThrowError(String::New("Callback is required and must be a Function."));
  }

  RenameBaton* baton = new RenameBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  NanAssignPersistent(Value, baton->refReference, args.This());
  baton->ref = ObjectWrap::Unwrap<GitReference>(args.This())->GetValue();
  NanAssignPersistent(Value, baton->new_nameReference, args[0]);
  const char * from_new_name;
  String::Utf8Value new_name(args[0]->ToString());
  from_new_name = strdup(*new_name);
  baton->new_name = from_new_name;
  NanAssignPersistent(Value, baton->forceReference, args[1]);
  int from_force;
  from_force = (int) args[1]->ToInt32()->Value();
  baton->force = from_force;
  NanAssignPersistent(Function, baton->callback, Local<Function>::Cast(args[2]));

  uv_queue_work(uv_default_loop(), &baton->request, RenameWork, (uv_after_work_cb)RenameAfterWork);

  NanReturnUndefined();
}

void GitReference::RenameWork(uv_work_t *req) {
  RenameBaton *baton = static_cast<RenameBaton *>(req->data);
  int result = git_reference_rename(
    &baton->out, 
    baton->ref, 
    baton->new_name, 
    baton->force
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitReference::RenameAfterWork(uv_work_t *req) {
  NanScope();
  RenameBaton *baton = static_cast<RenameBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
  if (baton->out != NULL) {
    to = GitReference::New((void *)baton->out);
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
  baton->refReference.Dispose();
  baton->new_nameReference.Dispose();
  baton->forceReference.Dispose();
  baton->callback.Dispose();
  free((void *)baton->new_name);
  delete baton;
}

#include "../include/functions/copy.h"

/**
 */
NAN_METHOD(GitReference::Delete) {
  NanScope();
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return NanThrowError(String::New("Callback is required and must be a Function."));
  }

  DeleteBaton* baton = new DeleteBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  NanAssignPersistent(Value, baton->refReference, Local<Function>::Cast(args.This()));
  baton->ref = ObjectWrap::Unwrap<GitReference>(args.This())->GetValue();
  NanAssignPersistent(Function, baton->callback, Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, DeleteWork, (uv_after_work_cb)DeleteAfterWork);

  NanReturnUndefined();
}

void GitReference::DeleteWork(uv_work_t *req) {
  DeleteBaton *baton = static_cast<DeleteBaton *>(req->data);
  int result = git_reference_delete(
    baton->ref
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitReference::DeleteAfterWork(uv_work_t *req) {
  NanScope();
  DeleteBaton *baton = static_cast<DeleteBaton *>(req->data);

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
  baton->refReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

/**
 */
NAN_METHOD(GitReference::IsBranch) {
  NanScope();

  int result = git_reference_is_branch(
    ObjectWrap::Unwrap<GitReference>(args.This())->GetValue()
  );
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
 */
NAN_METHOD(GitReference::IsRemote) {
  NanScope();
  

  int result = git_reference_is_remote(
    ObjectWrap::Unwrap<GitReference>(args.This())->GetValue()
  );
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
 * @param {Number} type
 * @return {Object} out
 */
NAN_METHOD(GitReference::Peel) {
  NanScope();
  if (args.Length() == 0 || !args[0]->IsInt32()) {
    return NanThrowError(String::New("Number type is required."));
  }

  git_object * out = 0;
  git_otype from_type;
            from_type = (git_otype) args[0]->ToInt32()->Value();
      
  int result = git_reference_peel(
    &out
    , ObjectWrap::Unwrap<GitReference>(args.This())->GetValue()
    , from_type
  );
  if (result != GIT_OK) {
    if (giterr_last()) {
      return NanThrowError(String::New(giterr_last()->message));
    } else {
      return NanThrowError(String::New("Unkown Error"));
    }
  }

  Handle<Value> to;
    if (out != NULL) {
    to = GitObject::New((void *)out);
  } else {
    to = Null();
  }
  NanReturnValue(to);
}

/**
 * @param {String} refname
 */
NAN_METHOD(GitReference::IsValidName) {
  NanScope();
  if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError(String::New("String refname is required."));
  }

  const char * from_refname;
            String::Utf8Value refname(args[0]->ToString());
      from_refname = strdup(*refname);
      
  int result = git_reference_is_valid_name(
    from_refname
  );
  free((void *)from_refname);
  if (result != GIT_OK) {
    if (giterr_last()) {
      return NanThrowError(String::New(giterr_last()->message));
    } else {
      return NanThrowError(String::New("Unkown Error"));
    }
  }

  NanReturnUndefined();
}

Persistent<FunctionTemplate> GitReference::constructor_template;

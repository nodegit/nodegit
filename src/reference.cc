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
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Reference"));

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


  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Reference"), constructor_template);
}

Handle<Value> GitReference::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return ThrowException(Exception::Error(String::New("git_reference is required.")));
  }

  GitReference* object = new GitReference((git_reference *) External::Unwrap(args[0]));
  object->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> GitReference::New(void *raw) {
  HandleScope scope;
  Handle<Value> argv[1] = { External::New((void *)raw) };
  return scope.Close(GitReference::constructor_template->NewInstance(1, argv));
}

git_reference *GitReference::GetValue() {
  return this->raw;
}


/**
 * @param {Repository} repo
 * @param {String} name
 * @param {Oid} callback
 */
Handle<Value> GitReference::OidForName(const Arguments& args) {
  HandleScope scope;
      if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository repo is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsString()) {
    return ThrowException(Exception::Error(String::New("String name is required.")));
  }

  if (args.Length() == 2 || !args[2]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  OidForNameBaton* baton = new OidForNameBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->out = (git_oid *)malloc(sizeof(git_oid ));
  baton->repoReference = Persistent<Value>::New(args[0]);
    git_repository * from_repo;
            from_repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
          baton->repo = from_repo;
    baton->nameReference = Persistent<Value>::New(args[1]);
    const char * from_name;
            String::Utf8Value name(args[1]->ToString());
      from_name = strdup(*name);
          baton->name = from_name;
    baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[2]));

  uv_queue_work(uv_default_loop(), &baton->request, OidForNameWork, (uv_after_work_cb)OidForNameAfterWork);

  return Undefined();
}

void GitReference::OidForNameWork(uv_work_t *req) {
  OidForNameBaton *baton = static_cast<OidForNameBaton *>(req->data);
  int result = git_reference_name_to_id(
    baton->out, 
    baton->repo, 
    baton->name
  );
  baton->error_code = result;
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitReference::OidForNameAfterWork(uv_work_t *req) {
  HandleScope scope;
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
    } else {
      baton->callback->Call(Context::GetCurrent()->Global(), 0, NULL);
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
Handle<Value> GitReference::Target(const Arguments& args) {
  HandleScope scope;
  

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
  return scope.Close(to);
}

/**
 * @return {String} result
 */
Handle<Value> GitReference::SymbolicTarget(const Arguments& args) {
  HandleScope scope;
  

  const char * result = git_reference_symbolic_target(
    ObjectWrap::Unwrap<GitReference>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = String::New(result);
  return scope.Close(to);
}

/**
 * @return {Number} result
 */
Handle<Value> GitReference::Type(const Arguments& args) {
  HandleScope scope;
  

  git_ref_t result = git_reference_type(
    ObjectWrap::Unwrap<GitReference>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = Number::New(result);
  return scope.Close(to);
}

/**
 * @return {String} result
 */
Handle<Value> GitReference::Name(const Arguments& args) {
  HandleScope scope;
  

  const char * result = git_reference_name(
    ObjectWrap::Unwrap<GitReference>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = String::New(result);
  return scope.Close(to);
}

/**
 * @param {Reference} callback
 */
Handle<Value> GitReference::Resolve(const Arguments& args) {
  HandleScope scope;
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  ResolveBaton* baton = new ResolveBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->refReference = Persistent<Value>::New(args.This());
  baton->ref = ObjectWrap::Unwrap<GitReference>(args.This())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, ResolveWork, (uv_after_work_cb)ResolveAfterWork);

  return Undefined();
}

void GitReference::ResolveWork(uv_work_t *req) {
  ResolveBaton *baton = static_cast<ResolveBaton *>(req->data);
  int result = git_reference_resolve(
    &baton->out, 
    baton->ref
  );
  baton->error_code = result;
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitReference::ResolveAfterWork(uv_work_t *req) {
  HandleScope scope;
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
    } else {
      baton->callback->Call(Context::GetCurrent()->Global(), 0, NULL);
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
Handle<Value> GitReference::SetSymbolicTarget(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String target is required.")));
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
      return ThrowException(Exception::Error(String::New(giterr_last()->message)));
    } else {
      return ThrowException(Exception::Error(String::New("Unkown Error")));
    }
  }

  Handle<Value> to;
    if (out != NULL) {
    to = GitReference::New((void *)out);
  } else {
    to = Null();
  }
  return scope.Close(to);
}

/**
 * @param {Oid} id
 * @return {Reference} out
 */
Handle<Value> GitReference::setTarget(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid id is required.")));
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
      return ThrowException(Exception::Error(String::New(giterr_last()->message)));
    } else {
      return ThrowException(Exception::Error(String::New("Unkown Error")));
    }
  }

  Handle<Value> to;
    if (out != NULL) {
    to = GitReference::New((void *)out);
  } else {
    to = Null();
  }
  return scope.Close(to);
}

/**
 * @param {String} new_name
 * @param {Number} force
 * @param {Reference} callback
 */
Handle<Value> GitReference::Rename(const Arguments& args) {
  HandleScope scope;
      if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String new_name is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number force is required.")));
  }

  if (args.Length() == 2 || !args[2]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  RenameBaton* baton = new RenameBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->refReference = Persistent<Value>::New(args.This());
  baton->ref = ObjectWrap::Unwrap<GitReference>(args.This())->GetValue();
  baton->new_nameReference = Persistent<Value>::New(args[0]);
    const char * from_new_name;
            String::Utf8Value new_name(args[0]->ToString());
      from_new_name = strdup(*new_name);
          baton->new_name = from_new_name;
    baton->forceReference = Persistent<Value>::New(args[1]);
    int from_force;
            from_force = (int) args[1]->ToInt32()->Value();
          baton->force = from_force;
    baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[2]));

  uv_queue_work(uv_default_loop(), &baton->request, RenameWork, (uv_after_work_cb)RenameAfterWork);

  return Undefined();
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
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitReference::RenameAfterWork(uv_work_t *req) {
  HandleScope scope;
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
    } else {
      baton->callback->Call(Context::GetCurrent()->Global(), 0, NULL);
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

/**
 */
Handle<Value> GitReference::Delete(const Arguments& args) {
  HandleScope scope;
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  DeleteBaton* baton = new DeleteBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->refReference = Persistent<Value>::New(args.This());
  baton->ref = ObjectWrap::Unwrap<GitReference>(args.This())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, DeleteWork, (uv_after_work_cb)DeleteAfterWork);

  return Undefined();
}

void GitReference::DeleteWork(uv_work_t *req) {
  DeleteBaton *baton = static_cast<DeleteBaton *>(req->data);
  int result = git_reference_delete(
    baton->ref
  );
  baton->error_code = result;
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitReference::DeleteAfterWork(uv_work_t *req) {
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
  } else {
    if (baton->error) {
      Handle<Value> argv[1] = {
        Exception::Error(String::New(baton->error->message))
      };
      baton->callback->Call(Context::GetCurrent()->Global(), 1, argv);
    } else {
      baton->callback->Call(Context::GetCurrent()->Global(), 0, NULL);
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
Handle<Value> GitReference::IsBranch(const Arguments& args) {
  HandleScope scope;
  

  int result = git_reference_is_branch(
    ObjectWrap::Unwrap<GitReference>(args.This())->GetValue()
  );
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
 */
Handle<Value> GitReference::IsRemote(const Arguments& args) {
  HandleScope scope;
  

  int result = git_reference_is_remote(
    ObjectWrap::Unwrap<GitReference>(args.This())->GetValue()
  );
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
 * @param {Number} type
 * @return {Object} out
 */
Handle<Value> GitReference::Peel(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number type is required.")));
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
      return ThrowException(Exception::Error(String::New(giterr_last()->message)));
    } else {
      return ThrowException(Exception::Error(String::New("Unkown Error")));
    }
  }

  Handle<Value> to;
    if (out != NULL) {
    to = GitObject::New((void *)out);
  } else {
    to = Null();
  }
  return scope.Close(to);
}

/**
 * @param {String} refname
 */
Handle<Value> GitReference::IsValidName(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String refname is required.")));
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
      return ThrowException(Exception::Error(String::New(giterr_last()->message)));
    } else {
      return ThrowException(Exception::Error(String::New("Unkown Error")));
    }
  }

  return Undefined();
}

Persistent<Function> GitReference::constructor_template;

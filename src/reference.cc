/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <nan.h>
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

  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanNew<String>("Reference"));

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


  Local<Function> _constructor_template = tpl->GetFunction();
  NanAssignPersistent(constructor_template, _constructor_template);
  target->Set(NanNew<String>("Reference"), _constructor_template);
}

NAN_METHOD(GitReference::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError("git_reference is required.");
  }
  GitReference* object = new GitReference(static_cast<git_reference *>(Handle<External>::Cast(args[0])->Value()));
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitReference::New(void *raw) {
  NanEscapableScope();
  Handle<Value> argv[1] = { NanNew<External>((void *)raw) };
  return NanEscapeScope(NanNew<Function>(GitReference::constructor_template)->NewInstance(1, argv));
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
    return NanThrowError("Repository repo is required.");
  }
  if (args.Length() == 1 || !args[1]->IsString()) {
    return NanThrowError("String name is required.");
  }

  if (args.Length() == 2 || !args[2]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  OidForNameBaton* baton = new OidForNameBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->out = (git_oid *)malloc(sizeof(git_oid ));
    git_repository * from_repo;
            from_repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
          baton->repo = from_repo;
      const char * from_name;
            String::Utf8Value name(args[1]->ToString());
      from_name = strdup(*name);
          baton->name = from_name;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[2]));
  OidForNameWorker *worker = new OidForNameWorker(baton, callback);
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("repo", args[0]->ToObject());
  if (!args[1]->IsUndefined() && !args[1]->IsNull())
    worker->SaveToPersistent("name", args[1]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitReference::OidForNameWorker::Execute() {
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

void GitReference::OidForNameWorker::HandleOKCallback() {
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
    to = NanNull();
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
    to = NanNew<String>(result);
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
    to = NanNew<Number>(result);
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
    to = NanNew<String>(result);
  NanReturnValue(to);
}

#include "../include/functions/copy.h"

/**
 * @param {Reference} callback
 */
NAN_METHOD(GitReference::Resolve) {
  NanScope();
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  ResolveBaton* baton = new ResolveBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->ref = ObjectWrap::Unwrap<GitReference>(args.This())->GetValue();

  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[0]));
  ResolveWorker *worker = new ResolveWorker(baton, callback);
  worker->SaveToPersistent("ref", args.This());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitReference::ResolveWorker::Execute() {
  int result = git_reference_resolve(
    &baton->out, 
    baton->ref
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitReference::ResolveWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->out != NULL) {
    to = GitReference::New((void *)baton->out);
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

/**
 * @param {String} target
 * @return {Reference} out
 */
NAN_METHOD(GitReference::SetSymbolicTarget) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError("String target is required.");
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
      return NanThrowError(giterr_last()->message);
    } else {
      return NanThrowError("Unknown Error");
    }
  }

  Handle<Value> to;
    if (out != NULL) {
    to = GitReference::New((void *)out);
  } else {
    to = NanNull();
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
    return NanThrowError("Oid id is required.");
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
      return NanThrowError(giterr_last()->message);
    } else {
      return NanThrowError("Unknown Error");
    }
  }

  Handle<Value> to;
    if (out != NULL) {
    to = GitReference::New((void *)out);
  } else {
    to = NanNull();
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
    return NanThrowError("String new_name is required.");
  }
  if (args.Length() == 1 || !args[1]->IsInt32()) {
    return NanThrowError("Number force is required.");
  }

  if (args.Length() == 2 || !args[2]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  RenameBaton* baton = new RenameBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->ref = ObjectWrap::Unwrap<GitReference>(args.This())->GetValue();
    const char * from_new_name;
            String::Utf8Value new_name(args[0]->ToString());
      from_new_name = strdup(*new_name);
          baton->new_name = from_new_name;
      int from_force;
            from_force = (int)   args[1]->ToInt32()->Value();
          baton->force = from_force;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[2]));
  RenameWorker *worker = new RenameWorker(baton, callback);
  worker->SaveToPersistent("ref", args.This());
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("new_name", args[0]->ToObject());
  if (!args[1]->IsUndefined() && !args[1]->IsNull())
    worker->SaveToPersistent("force", args[1]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitReference::RenameWorker::Execute() {
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

void GitReference::RenameWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->out != NULL) {
    to = GitReference::New((void *)baton->out);
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
  free((void *)baton->new_name);
  delete baton;
}

#include "../include/functions/copy.h"

/**
 */
NAN_METHOD(GitReference::Delete) {
  NanScope();
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  DeleteBaton* baton = new DeleteBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->ref = ObjectWrap::Unwrap<GitReference>(args.This())->GetValue();

  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[0]));
  DeleteWorker *worker = new DeleteWorker(baton, callback);
  worker->SaveToPersistent("ref", args.This());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitReference::DeleteWorker::Execute() {
  int result = git_reference_delete(
    baton->ref
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitReference::DeleteWorker::HandleOKCallback() {
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
 */
NAN_METHOD(GitReference::IsBranch) {
  NanScope();
  

  int result = git_reference_is_branch(
    ObjectWrap::Unwrap<GitReference>(args.This())->GetValue()
  );
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
 */
NAN_METHOD(GitReference::IsRemote) {
  NanScope();
  

  int result = git_reference_is_remote(
    ObjectWrap::Unwrap<GitReference>(args.This())->GetValue()
  );
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
 * @param {Number} type
 * @return {Object} out
 */
NAN_METHOD(GitReference::Peel) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsInt32()) {
    return NanThrowError("Number type is required.");
  }

  git_object * out = 0;
  git_otype from_type;
            from_type = (git_otype)   args[0]->ToInt32()->Value();
      
  int result = git_reference_peel(
    &out
    , ObjectWrap::Unwrap<GitReference>(args.This())->GetValue()
    , from_type
  );
  if (result != GIT_OK) {
    if (giterr_last()) {
      return NanThrowError(giterr_last()->message);
    } else {
      return NanThrowError("Unknown Error");
    }
  }

  Handle<Value> to;
    if (out != NULL) {
    to = GitObject::New((void *)out);
  } else {
    to = NanNull();
  }
  NanReturnValue(to);
}

/**
 * @param {String} refname
 */
NAN_METHOD(GitReference::IsValidName) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError("String refname is required.");
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
      return NanThrowError(giterr_last()->message);
    } else {
      return NanThrowError("Unknown Error");
    }
  }

  NanReturnUndefined();
}

Persistent<Function> GitReference::constructor_template;

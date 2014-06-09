/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <nan.h>
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
#include "../include/remote.h"
#include "../include/clone_options.h"
#include "node_buffer.h"

using namespace v8;
using namespace node;

GitRepo::GitRepo(git_repository *raw) {
  this->raw = raw;
}

GitRepo::~GitRepo() {
  git_repository_free(this->raw);
}

void GitRepo::Initialize(Handle<v8::Object> target) {
  NanScope();

  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanNew<String>("Repo"));

  NODE_SET_METHOD(tpl, "open", Open);
  NODE_SET_METHOD(tpl, "init", Init);
  NODE_SET_PROTOTYPE_METHOD(tpl, "path", Path);
  NODE_SET_PROTOTYPE_METHOD(tpl, "workdir", Workdir);
  NODE_SET_PROTOTYPE_METHOD(tpl, "odb", Odb);
  NODE_SET_PROTOTYPE_METHOD(tpl, "openIndex", openIndex);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getBlob", GetBlob);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getCommit", GetCommit);
  NODE_SET_PROTOTYPE_METHOD(tpl, "createCommit", CreateCommit);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getObject", GetObject);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getReference", GetReference);
  NODE_SET_PROTOTYPE_METHOD(tpl, "createSymbolicReference", CreateSymbolicReference);
  NODE_SET_PROTOTYPE_METHOD(tpl, "createReference", CreateReference);
  NODE_SET_PROTOTYPE_METHOD(tpl, "addRemote", AddRemote);
  NODE_SET_PROTOTYPE_METHOD(tpl, "createRevWalk", CreateRevWalk);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getSubmodule", GetSubmodule);
  NODE_SET_PROTOTYPE_METHOD(tpl, "addSubmodule", AddSubmodule);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getTag", GetTag);
  NODE_SET_PROTOTYPE_METHOD(tpl, "createTag", CreateTag);
  NODE_SET_METHOD(tpl, "createLightweightTag", CreateLightweightTag);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getTree", GetTree);
  NODE_SET_METHOD(tpl, "reloadSubmodules", ReloadSubmodules);
  NODE_SET_PROTOTYPE_METHOD(tpl, "delete", Delete);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getReferences", GetReferences);
  NODE_SET_PROTOTYPE_METHOD(tpl, "createBlobFromBuffer", CreateBlobFromBuffer);
  NODE_SET_PROTOTYPE_METHOD(tpl, "createBlobFromFile", CreateBlobFromFile);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getRemotes", GetRemotes);
  NODE_SET_METHOD(tpl, "clone", Clone);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getRemote", GetRemote);


  Local<Function> _constructor_template = tpl->GetFunction();
  NanAssignPersistent(constructor_template, _constructor_template);
  target->Set(NanNew<String>("Repo"), _constructor_template);
}

NAN_METHOD(GitRepo::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError("git_repository is required.");
  }
  GitRepo* object = new GitRepo(static_cast<git_repository *>(Handle<External>::Cast(args[0])->Value()));
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitRepo::New(void *raw) {
  NanEscapableScope();
  Handle<Value> argv[1] = { NanNew<External>((void *)raw) };
  return NanEscapeScope(NanNew<Function>(GitRepo::constructor_template)->NewInstance(1, argv));
}

git_repository *GitRepo::GetValue() {
  return this->raw;
}


#include "../include/functions/copy.h"

/**
 * @param {String} path
 * @param {Repository} callback
 */
NAN_METHOD(GitRepo::Open) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError("String path is required.");
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  OpenBaton* baton = new OpenBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
    const char * from_path;
            String::Utf8Value path(args[0]->ToString());
      from_path = strdup(*path);
          baton->path = from_path;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[1]));
  OpenWorker *worker = new OpenWorker(baton, callback);
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("path", args[0]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitRepo::OpenWorker::Execute() {
  int result = git_repository_open(
    &baton->out, 
    baton->path
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRepo::OpenWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->out != NULL) {
    to = GitRepo::New((void *)baton->out);
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
  free((void *)baton->path);
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {String} path
 * @param {Boolean} is_bare
 * @param {Repository} callback
 */
NAN_METHOD(GitRepo::Init) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError("String path is required.");
  }
  if (args.Length() == 1 || !args[1]->IsBoolean()) {
    return NanThrowError("Boolean is_bare is required.");
  }

  if (args.Length() == 2 || !args[2]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  InitBaton* baton = new InitBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
    const char * from_path;
            String::Utf8Value path(args[0]->ToString());
      from_path = strdup(*path);
          baton->path = from_path;
      unsigned from_is_bare;
            from_is_bare = (unsigned)   args[1]->ToBoolean()->Value();
          baton->is_bare = from_is_bare;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[2]));
  InitWorker *worker = new InitWorker(baton, callback);
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("path", args[0]->ToObject());
  if (!args[1]->IsUndefined() && !args[1]->IsNull())
    worker->SaveToPersistent("is_bare", args[1]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitRepo::InitWorker::Execute() {
  int result = git_repository_init(
    &baton->out, 
    baton->path, 
    baton->is_bare
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRepo::InitWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->out != NULL) {
    to = GitRepo::New((void *)baton->out);
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
  free((void *)baton->path);
  delete baton;
}

/**
 * @return {String} result
 */
NAN_METHOD(GitRepo::Path) {
  NanScope();
  

  const char * result = git_repository_path(
    ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = NanNew<String>(result);
  NanReturnValue(to);
}

/**
 * @return {String} result
 */
NAN_METHOD(GitRepo::Workdir) {
  NanScope();
  

  const char * result = git_repository_workdir(
    ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = NanNew<String>(result);
  NanReturnValue(to);
}

/**
 * @return {Odb} out
 */
NAN_METHOD(GitRepo::Odb) {
  NanScope();
  
  git_odb * out = 0;

  int result = git_repository_odb(
    &out
    , ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue()
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
    to = GitOdb::New((void *)out);
  } else {
    to = NanNull();
  }
  NanReturnValue(to);
}

#include "../include/functions/copy.h"

/**
 * @param {Index} callback
 */
NAN_METHOD(GitRepo::openIndex) {
  NanScope();
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  openIndexBaton* baton = new openIndexBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue();

  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[0]));
  openIndexWorker *worker = new openIndexWorker(baton, callback);
  worker->SaveToPersistent("repo", args.This());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitRepo::openIndexWorker::Execute() {
  int result = git_repository_index(
    &baton->out, 
    baton->repo
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRepo::openIndexWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->out != NULL) {
    to = GitIndex::New((void *)baton->out);
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
 * @param {Oid} id
 * @param {Blob} callback
 */
NAN_METHOD(GitRepo::GetBlob) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsObject()) {
    return NanThrowError("Oid id is required.");
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  GetBlobBaton* baton = new GetBlobBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue();
    const git_oid * from_id;
            from_id = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject())->GetValue();
          baton->id = from_id;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[1]));
  GetBlobWorker *worker = new GetBlobWorker(baton, callback);
  worker->SaveToPersistent("repo", args.This());
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("id", args[0]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitRepo::GetBlobWorker::Execute() {
  int result = git_blob_lookup(
    &baton->blob, 
    baton->repo, 
    baton->id
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRepo::GetBlobWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->blob != NULL) {
    to = GitBlob::New((void *)baton->blob);
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
 * @param {Oid} id
 * @param {Commit} callback
 */
NAN_METHOD(GitRepo::GetCommit) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsObject()) {
    return NanThrowError("Oid id is required.");
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  GetCommitBaton* baton = new GetCommitBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue();
    const git_oid * from_id;
            from_id = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject())->GetValue();
          baton->id = from_id;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[1]));
  GetCommitWorker *worker = new GetCommitWorker(baton, callback);
  worker->SaveToPersistent("repo", args.This());
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("id", args[0]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitRepo::GetCommitWorker::Execute() {
  int result = git_commit_lookup(
    &baton->commit, 
    baton->repo, 
    baton->id
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRepo::GetCommitWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->commit != NULL) {
    to = GitCommit::New((void *)baton->commit);
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
 * @param {String} update_ref
 * @param {Signature} author
 * @param {Signature} committer
 * @param {String} message_encoding
 * @param {String} message
 * @param {Tree} tree
 * @param {Number} parent_count
 * @param {Array} parents
 * @param {Oid} callback
 */
NAN_METHOD(GitRepo::CreateCommit) {
  NanScope();
      if (args.Length() == 1 || !args[1]->IsObject()) {
    return NanThrowError("Signature author is required.");
  }
  if (args.Length() == 2 || !args[2]->IsObject()) {
    return NanThrowError("Signature committer is required.");
  }
  if (args.Length() == 4 || !args[4]->IsString()) {
    return NanThrowError("String message is required.");
  }
  if (args.Length() == 5 || !args[5]->IsObject()) {
    return NanThrowError("Tree tree is required.");
  }
  if (args.Length() == 6 || !args[6]->IsInt32()) {
    return NanThrowError("Number parent_count is required.");
  }
  if (args.Length() == 7 || !args[7]->IsObject()) {
    return NanThrowError("Array parents is required.");
  }

  if (args.Length() == 8 || !args[8]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  CreateCommitBaton* baton = new CreateCommitBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->id = (git_oid *)malloc(sizeof(git_oid ));
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue();
    const char * from_update_ref;
      if (args[0]->IsString()) {
            String::Utf8Value update_ref(args[0]->ToString());
      from_update_ref = strdup(*update_ref);
          } else {
      from_update_ref = 0;
    }
      baton->update_ref = from_update_ref;
      const git_signature * from_author;
            from_author = ObjectWrap::Unwrap<GitSignature>(args[1]->ToObject())->GetValue();
          baton->author = from_author;
      const git_signature * from_committer;
            from_committer = ObjectWrap::Unwrap<GitSignature>(args[2]->ToObject())->GetValue();
          baton->committer = from_committer;
      const char * from_message_encoding;
      if (args[3]->IsString()) {
            String::Utf8Value message_encoding(args[3]->ToString());
      from_message_encoding = strdup(*message_encoding);
          } else {
      from_message_encoding = 0;
    }
      baton->message_encoding = from_message_encoding;
      const char * from_message;
            String::Utf8Value message(args[4]->ToString());
      from_message = strdup(*message);
          baton->message = from_message;
      const git_tree * from_tree;
            from_tree = ObjectWrap::Unwrap<GitTree>(args[5]->ToObject())->GetValue();
          baton->tree = from_tree;
      int from_parent_count;
            from_parent_count = (int)   args[6]->ToInt32()->Value();
          baton->parent_count = from_parent_count;
      const git_commit ** from_parents;
            Array *tmp_parents = Array::Cast(*args[7]);
      from_parents = (const git_commit **)malloc(tmp_parents->Length() * sizeof(const git_commit *));
      for (unsigned int i = 0; i < tmp_parents->Length(); i++) {
    
        from_parents[i] = ObjectWrap::Unwrap<GitCommit>(tmp_parents->Get(NanNew<Number>(static_cast<double>(i)))->ToObject())->GetValue();
      }
          baton->parents = from_parents;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[8]));
  CreateCommitWorker *worker = new CreateCommitWorker(baton, callback);
  worker->SaveToPersistent("repo", args.This());
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("update_ref", args[0]->ToObject());
  if (!args[1]->IsUndefined() && !args[1]->IsNull())
    worker->SaveToPersistent("author", args[1]->ToObject());
  if (!args[2]->IsUndefined() && !args[2]->IsNull())
    worker->SaveToPersistent("committer", args[2]->ToObject());
  if (!args[3]->IsUndefined() && !args[3]->IsNull())
    worker->SaveToPersistent("message_encoding", args[3]->ToObject());
  if (!args[4]->IsUndefined() && !args[4]->IsNull())
    worker->SaveToPersistent("message", args[4]->ToObject());
  if (!args[5]->IsUndefined() && !args[5]->IsNull())
    worker->SaveToPersistent("tree", args[5]->ToObject());
  if (!args[6]->IsUndefined() && !args[6]->IsNull())
    worker->SaveToPersistent("parent_count", args[6]->ToObject());
  if (!args[7]->IsUndefined() && !args[7]->IsNull())
    worker->SaveToPersistent("parents", args[7]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitRepo::CreateCommitWorker::Execute() {
  int result = git_commit_create(
    baton->id, 
    baton->repo, 
    baton->update_ref, 
    baton->author, 
    baton->committer, 
    baton->message_encoding, 
    baton->message, 
    baton->tree, 
    baton->parent_count, 
    baton->parents
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRepo::CreateCommitWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->id != NULL) {
    to = GitOid::New((void *)baton->id);
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
        free(baton->id);
      }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  free((void *)baton->update_ref);
  free((void *)baton->message_encoding);
  free((void *)baton->message);
  free((void *)baton->parents);
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {Oid} id
 * @param {Number} type
 * @param {Object} callback
 */
NAN_METHOD(GitRepo::GetObject) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsObject()) {
    return NanThrowError("Oid id is required.");
  }
  if (args.Length() == 1 || !args[1]->IsInt32()) {
    return NanThrowError("Number type is required.");
  }

  if (args.Length() == 2 || !args[2]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  GetObjectBaton* baton = new GetObjectBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue();
    const git_oid * from_id;
            from_id = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject())->GetValue();
          baton->id = from_id;
      git_otype from_type;
            from_type = (git_otype)   args[1]->ToInt32()->Value();
          baton->type = from_type;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[2]));
  GetObjectWorker *worker = new GetObjectWorker(baton, callback);
  worker->SaveToPersistent("repo", args.This());
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("id", args[0]->ToObject());
  if (!args[1]->IsUndefined() && !args[1]->IsNull())
    worker->SaveToPersistent("type", args[1]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitRepo::GetObjectWorker::Execute() {
  int result = git_object_lookup(
    &baton->object, 
    baton->repo, 
    baton->id, 
    baton->type
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRepo::GetObjectWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->object != NULL) {
    to = GitObject::New((void *)baton->object);
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
 * @param {String} name
 * @param {Reference} callback
 */
NAN_METHOD(GitRepo::GetReference) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError("String name is required.");
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  GetReferenceBaton* baton = new GetReferenceBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue();
    const char * from_name;
            String::Utf8Value name(args[0]->ToString());
      from_name = strdup(*name);
          baton->name = from_name;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[1]));
  GetReferenceWorker *worker = new GetReferenceWorker(baton, callback);
  worker->SaveToPersistent("repo", args.This());
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("name", args[0]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitRepo::GetReferenceWorker::Execute() {
  int result = git_reference_lookup(
    &baton->out, 
    baton->repo, 
    baton->name
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRepo::GetReferenceWorker::HandleOKCallback() {
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
  free((void *)baton->name);
  delete baton;
}

/**
 * @param {String} name
 * @param {String} target
 * @param {Number} force
 * @return {Reference} out
 */
NAN_METHOD(GitRepo::CreateSymbolicReference) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError("String name is required.");
  }
  if (args.Length() == 1 || !args[1]->IsString()) {
    return NanThrowError("String target is required.");
  }
  if (args.Length() == 2 || !args[2]->IsInt32()) {
    return NanThrowError("Number force is required.");
  }

  git_reference * out = 0;
  const char * from_name;
            String::Utf8Value name(args[0]->ToString());
      from_name = strdup(*name);
        const char * from_target;
            String::Utf8Value target(args[1]->ToString());
      from_target = strdup(*target);
        int from_force;
            from_force = (int)   args[2]->ToInt32()->Value();
      
  int result = git_reference_symbolic_create(
    &out
    , ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue()
    , from_name
    , from_target
    , from_force
  );
  free((void *)from_name);
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
 * @param {String} name
 * @param {Oid} id
 * @param {Number} force
 * @return {Reference} out
 */
NAN_METHOD(GitRepo::CreateReference) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError("String name is required.");
  }
  if (args.Length() == 1 || !args[1]->IsObject()) {
    return NanThrowError("Oid id is required.");
  }
  if (args.Length() == 2 || !args[2]->IsInt32()) {
    return NanThrowError("Number force is required.");
  }

  git_reference * out = 0;
  const char * from_name;
            String::Utf8Value name(args[0]->ToString());
      from_name = strdup(*name);
        const git_oid * from_id;
            from_id = ObjectWrap::Unwrap<GitOid>(args[1]->ToObject())->GetValue();
        int from_force;
            from_force = (int)   args[2]->ToInt32()->Value();
      
  int result = git_reference_create(
    &out
    , ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue()
    , from_name
    , from_id
    , from_force
  );
  free((void *)from_name);
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
 * @param {String} name
 * @param {String} url
 * @param {Remote} callback
 */
NAN_METHOD(GitRepo::AddRemote) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError("String name is required.");
  }
  if (args.Length() == 1 || !args[1]->IsString()) {
    return NanThrowError("String url is required.");
  }

  if (args.Length() == 2 || !args[2]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  AddRemoteBaton* baton = new AddRemoteBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue();
    const char * from_name;
            String::Utf8Value name(args[0]->ToString());
      from_name = strdup(*name);
          baton->name = from_name;
      const char * from_url;
            String::Utf8Value url(args[1]->ToString());
      from_url = strdup(*url);
          baton->url = from_url;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[2]));
  AddRemoteWorker *worker = new AddRemoteWorker(baton, callback);
  worker->SaveToPersistent("repo", args.This());
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("name", args[0]->ToObject());
  if (!args[1]->IsUndefined() && !args[1]->IsNull())
    worker->SaveToPersistent("url", args[1]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitRepo::AddRemoteWorker::Execute() {
  int result = git_remote_create(
    &baton->out, 
    baton->repo, 
    baton->name, 
    baton->url
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRepo::AddRemoteWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->out != NULL) {
    to = GitRemote::New((void *)baton->out);
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
  free((void *)baton->name);
  free((void *)baton->url);
  delete baton;
}

/**
 * @return {RevWalk} out
 */
NAN_METHOD(GitRepo::CreateRevWalk) {
  NanScope();
  
  git_revwalk * out = 0;

  int result = git_revwalk_new(
    &out
    , ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue()
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
    to = GitRevWalk::New((void *)out);
  } else {
    to = NanNull();
  }
  NanReturnValue(to);
}

/**
 * @param {String} name
 * @return {Submodule} submodule
 */
NAN_METHOD(GitRepo::GetSubmodule) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError("String name is required.");
  }

  git_submodule * submodule = 0;
  const char * from_name;
            String::Utf8Value name(args[0]->ToString());
      from_name = strdup(*name);
      
  int result = git_submodule_lookup(
    &submodule
    , ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue()
    , from_name
  );
  free((void *)from_name);
  if (result != GIT_OK) {
    if (giterr_last()) {
      return NanThrowError(giterr_last()->message);
    } else {
      return NanThrowError("Unknown Error");
    }
  }

  Handle<Value> to;
    if (submodule != NULL) {
    to = GitSubmodule::New((void *)submodule);
  } else {
    to = NanNull();
  }
  NanReturnValue(to);
}

/**
 * @param {String} url
 * @param {String} path
 * @param {Number} use_gitlink
 * @return {Submodule} submodule
 */
NAN_METHOD(GitRepo::AddSubmodule) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError("String url is required.");
  }
  if (args.Length() == 1 || !args[1]->IsString()) {
    return NanThrowError("String path is required.");
  }
  if (args.Length() == 2 || !args[2]->IsInt32()) {
    return NanThrowError("Number use_gitlink is required.");
  }

  git_submodule * submodule = 0;
  const char * from_url;
            String::Utf8Value url(args[0]->ToString());
      from_url = strdup(*url);
        const char * from_path;
            String::Utf8Value path(args[1]->ToString());
      from_path = strdup(*path);
        int from_use_gitlink;
            from_use_gitlink = (int)   args[2]->ToInt32()->Value();
      
  int result = git_submodule_add_setup(
    &submodule
    , ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue()
    , from_url
    , from_path
    , from_use_gitlink
  );
  free((void *)from_url);
  free((void *)from_path);
  if (result != GIT_OK) {
    if (giterr_last()) {
      return NanThrowError(giterr_last()->message);
    } else {
      return NanThrowError("Unknown Error");
    }
  }

  Handle<Value> to;
    if (submodule != NULL) {
    to = GitSubmodule::New((void *)submodule);
  } else {
    to = NanNull();
  }
  NanReturnValue(to);
}

#include "../include/functions/copy.h"

/**
 * @param {Oid} id
 * @param {Tag} callback
 */
NAN_METHOD(GitRepo::GetTag) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsObject()) {
    return NanThrowError("Oid id is required.");
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  GetTagBaton* baton = new GetTagBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue();
    const git_oid * from_id;
            from_id = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject())->GetValue();
          baton->id = from_id;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[1]));
  GetTagWorker *worker = new GetTagWorker(baton, callback);
  worker->SaveToPersistent("repo", args.This());
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("id", args[0]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitRepo::GetTagWorker::Execute() {
  int result = git_tag_lookup(
    &baton->out, 
    baton->repo, 
    baton->id
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRepo::GetTagWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->out != NULL) {
    to = GitTag::New((void *)baton->out);
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
 * @param {String} tag_name
 * @param {Object} target
 * @param {Signature} tagger
 * @param {String} message
 * @param {Number} force
 * @param {Oid} callback
 */
NAN_METHOD(GitRepo::CreateTag) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError("String tag_name is required.");
  }
  if (args.Length() == 1 || !args[1]->IsObject()) {
    return NanThrowError("Object target is required.");
  }
  if (args.Length() == 2 || !args[2]->IsObject()) {
    return NanThrowError("Signature tagger is required.");
  }
  if (args.Length() == 3 || !args[3]->IsString()) {
    return NanThrowError("String message is required.");
  }
  if (args.Length() == 4 || !args[4]->IsInt32()) {
    return NanThrowError("Number force is required.");
  }

  if (args.Length() == 5 || !args[5]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  CreateTagBaton* baton = new CreateTagBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->oid = (git_oid *)malloc(sizeof(git_oid ));
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue();
    const char * from_tag_name;
            String::Utf8Value tag_name(args[0]->ToString());
      from_tag_name = strdup(*tag_name);
          baton->tag_name = from_tag_name;
      const git_object * from_target;
            from_target = ObjectWrap::Unwrap<GitObject>(args[1]->ToObject())->GetValue();
          baton->target = from_target;
      const git_signature * from_tagger;
            from_tagger = ObjectWrap::Unwrap<GitSignature>(args[2]->ToObject())->GetValue();
          baton->tagger = from_tagger;
      const char * from_message;
            String::Utf8Value message(args[3]->ToString());
      from_message = strdup(*message);
          baton->message = from_message;
      int from_force;
            from_force = (int)   args[4]->ToInt32()->Value();
          baton->force = from_force;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[5]));
  CreateTagWorker *worker = new CreateTagWorker(baton, callback);
  worker->SaveToPersistent("repo", args.This());
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("tag_name", args[0]->ToObject());
  if (!args[1]->IsUndefined() && !args[1]->IsNull())
    worker->SaveToPersistent("target", args[1]->ToObject());
  if (!args[2]->IsUndefined() && !args[2]->IsNull())
    worker->SaveToPersistent("tagger", args[2]->ToObject());
  if (!args[3]->IsUndefined() && !args[3]->IsNull())
    worker->SaveToPersistent("message", args[3]->ToObject());
  if (!args[4]->IsUndefined() && !args[4]->IsNull())
    worker->SaveToPersistent("force", args[4]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitRepo::CreateTagWorker::Execute() {
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
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRepo::CreateTagWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->oid != NULL) {
    to = GitOid::New((void *)baton->oid);
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
        free(baton->oid);
      }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  free((void *)baton->tag_name);
  free((void *)baton->message);
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {String} tag_name
 * @param {Object} target
 * @param {Number} force
 * @param {Oid} callback
 */
NAN_METHOD(GitRepo::CreateLightweightTag) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError("String tag_name is required.");
  }
  if (args.Length() == 1 || !args[1]->IsObject()) {
    return NanThrowError("Object target is required.");
  }
  if (args.Length() == 2 || !args[2]->IsInt32()) {
    return NanThrowError("Number force is required.");
  }

  if (args.Length() == 3 || !args[3]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  CreateLightweightTagBaton* baton = new CreateLightweightTagBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->oid = (git_oid *)malloc(sizeof(git_oid ));
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue();
    const char * from_tag_name;
            String::Utf8Value tag_name(args[0]->ToString());
      from_tag_name = strdup(*tag_name);
          baton->tag_name = from_tag_name;
      const git_object * from_target;
            from_target = ObjectWrap::Unwrap<GitObject>(args[1]->ToObject())->GetValue();
          baton->target = from_target;
      int from_force;
            from_force = (int)   args[2]->ToInt32()->Value();
          baton->force = from_force;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[3]));
  CreateLightweightTagWorker *worker = new CreateLightweightTagWorker(baton, callback);
  worker->SaveToPersistent("repo", args.This());
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("tag_name", args[0]->ToObject());
  if (!args[1]->IsUndefined() && !args[1]->IsNull())
    worker->SaveToPersistent("target", args[1]->ToObject());
  if (!args[2]->IsUndefined() && !args[2]->IsNull())
    worker->SaveToPersistent("force", args[2]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitRepo::CreateLightweightTagWorker::Execute() {
  int result = git_tag_create_lightweight(
    baton->oid, 
    baton->repo, 
    baton->tag_name, 
    baton->target, 
    baton->force
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRepo::CreateLightweightTagWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->oid != NULL) {
    to = GitOid::New((void *)baton->oid);
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
        free(baton->oid);
      }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  free((void *)baton->tag_name);
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {Oid} id
 * @param {Tree} callback
 */
NAN_METHOD(GitRepo::GetTree) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsObject()) {
    return NanThrowError("Oid id is required.");
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  GetTreeBaton* baton = new GetTreeBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue();
    const git_oid * from_id;
            from_id = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject())->GetValue();
          baton->id = from_id;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[1]));
  GetTreeWorker *worker = new GetTreeWorker(baton, callback);
  worker->SaveToPersistent("repo", args.This());
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("id", args[0]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitRepo::GetTreeWorker::Execute() {
  int result = git_tree_lookup(
    &baton->out, 
    baton->repo, 
    baton->id
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRepo::GetTreeWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->out != NULL) {
    to = GitTree::New((void *)baton->out);
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
NAN_METHOD(GitRepo::ReloadSubmodules) {
  NanScope();
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  ReloadSubmodulesBaton* baton = new ReloadSubmodulesBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue();

  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[0]));
  ReloadSubmodulesWorker *worker = new ReloadSubmodulesWorker(baton, callback);
  worker->SaveToPersistent("repo", args.This());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitRepo::ReloadSubmodulesWorker::Execute() {
  int result = git_submodule_reload_all(
    baton->repo
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRepo::ReloadSubmodulesWorker::HandleOKCallback() {
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
 * @param {String} tag_name
 */
NAN_METHOD(GitRepo::Delete) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError("String tag_name is required.");
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  DeleteBaton* baton = new DeleteBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue();
    const char * from_tag_name;
            String::Utf8Value tag_name(args[0]->ToString());
      from_tag_name = strdup(*tag_name);
          baton->tag_name = from_tag_name;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[1]));
  DeleteWorker *worker = new DeleteWorker(baton, callback);
  worker->SaveToPersistent("repo", args.This());
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("tag_name", args[0]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitRepo::DeleteWorker::Execute() {
  int result = git_tag_delete(
    baton->repo, 
    baton->tag_name
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRepo::DeleteWorker::HandleOKCallback() {
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
  free((void *)baton->tag_name);
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {Number} list_flags
 * @param {Array} callback
 */
NAN_METHOD(GitRepo::GetReferences) {
  NanScope();
    
  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  GetReferencesBaton* baton = new GetReferencesBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->array = (git_strarray *)malloc(sizeof(git_strarray ));
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue();
    unsigned int from_list_flags;
      if (args[0]->IsUint32()) {
            from_list_flags = (unsigned int)   args[0]->ToUint32()->Value();
          } else {
      from_list_flags = 0;
    }
      baton->list_flags = from_list_flags;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[1]));
  GetReferencesWorker *worker = new GetReferencesWorker(baton, callback);
  worker->SaveToPersistent("repo", args.This());
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("list_flags", args[0]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitRepo::GetReferencesWorker::Execute() {
  int result = git_reference_list(
    baton->array, 
    baton->repo, 
    baton->list_flags
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRepo::GetReferencesWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
  
  Local<Array> tmpArray = NanNew<Array>(baton->array->count);
  for (unsigned int i = 0; i < baton->array->count; i++) {
    tmpArray->Set(NanNew<Number>(i), NanNew<String>(baton->array->strings[i]));
  }
  to = tmpArray;
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
        free(baton->array);
      }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }

  git_strarray_free(baton->array);
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {Buffer} buffer
 * @param {Number} len
 * @param {Oid} callback
 */
NAN_METHOD(GitRepo::CreateBlobFromBuffer) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsObject()) {
    return NanThrowError("Buffer buffer is required.");
  }
  if (args.Length() == 1 || !args[1]->IsNumber()) {
    return NanThrowError("Number len is required.");
  }

  if (args.Length() == 2 || !args[2]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  CreateBlobFromBufferBaton* baton = new CreateBlobFromBufferBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->oid = (git_oid *)malloc(sizeof(git_oid ));
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue();
    const void * from_buffer;
            from_buffer = Buffer::Data(args[0]->ToObject());
          baton->buffer = from_buffer;
      size_t from_len;
            from_len = (size_t)   args[1]->ToNumber()->Value();
          baton->len = from_len;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[2]));
  CreateBlobFromBufferWorker *worker = new CreateBlobFromBufferWorker(baton, callback);
  worker->SaveToPersistent("repo", args.This());
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("buffer", args[0]->ToObject());
  if (!args[1]->IsUndefined() && !args[1]->IsNull())
    worker->SaveToPersistent("len", args[1]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitRepo::CreateBlobFromBufferWorker::Execute() {
  int result = git_blob_create_frombuffer(
    baton->oid, 
    baton->repo, 
    baton->buffer, 
    baton->len
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRepo::CreateBlobFromBufferWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->oid != NULL) {
    to = GitOid::New((void *)baton->oid);
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
        free(baton->oid);
      }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {String} path
 * @param {Oid} callback
 */
NAN_METHOD(GitRepo::CreateBlobFromFile) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError("String path is required.");
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  CreateBlobFromFileBaton* baton = new CreateBlobFromFileBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->id = (git_oid *)malloc(sizeof(git_oid ));
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue();
    const char * from_path;
            String::Utf8Value path(args[0]->ToString());
      from_path = strdup(*path);
          baton->path = from_path;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[1]));
  CreateBlobFromFileWorker *worker = new CreateBlobFromFileWorker(baton, callback);
  worker->SaveToPersistent("repo", args.This());
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("path", args[0]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitRepo::CreateBlobFromFileWorker::Execute() {
  int result = git_blob_create_fromdisk(
    baton->id, 
    baton->repo, 
    baton->path
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRepo::CreateBlobFromFileWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->id != NULL) {
    to = GitOid::New((void *)baton->id);
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
        free(baton->id);
      }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  free((void *)baton->path);
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {Array} callback
 */
NAN_METHOD(GitRepo::GetRemotes) {
  NanScope();
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  GetRemotesBaton* baton = new GetRemotesBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->out = (git_strarray *)malloc(sizeof(git_strarray ));
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue();

  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[0]));
  GetRemotesWorker *worker = new GetRemotesWorker(baton, callback);
  worker->SaveToPersistent("repo", args.This());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitRepo::GetRemotesWorker::Execute() {
  int result = git_remote_list(
    baton->out, 
    baton->repo
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRepo::GetRemotesWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
  
  Local<Array> tmpArray = NanNew<Array>(baton->out->count);
  for (unsigned int i = 0; i < baton->out->count; i++) {
    tmpArray->Set(NanNew<Number>(i), NanNew<String>(baton->out->strings[i]));
  }
  to = tmpArray;
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

  git_strarray_free(baton->out);
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {String} url
 * @param {String} local_path
 * @param {CloneOptions} options
 * @param {Repository} callback
 */
NAN_METHOD(GitRepo::Clone) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError("String url is required.");
  }
  if (args.Length() == 1 || !args[1]->IsString()) {
    return NanThrowError("String local_path is required.");
  }

  if (args.Length() == 3 || !args[3]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  CloneBaton* baton = new CloneBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
    const char * from_url;
            String::Utf8Value url(args[0]->ToString());
      from_url = strdup(*url);
          baton->url = from_url;
      const char * from_local_path;
            String::Utf8Value local_path(args[1]->ToString());
      from_local_path = strdup(*local_path);
          baton->local_path = from_local_path;
      const git_clone_options * from_options;
      if (args[2]->IsObject()) {
            from_options = ObjectWrap::Unwrap<GitCloneOptions>(args[2]->ToObject())->GetValue();
          } else {
      from_options = 0;
    }
      baton->options = from_options;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[3]));
  CloneWorker *worker = new CloneWorker(baton, callback);
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("url", args[0]->ToObject());
  if (!args[1]->IsUndefined() && !args[1]->IsNull())
    worker->SaveToPersistent("local_path", args[1]->ToObject());
  if (!args[2]->IsUndefined() && !args[2]->IsNull())
    worker->SaveToPersistent("options", args[2]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitRepo::CloneWorker::Execute() {
  int result = git_clone(
    &baton->out, 
    baton->url, 
    baton->local_path, 
    baton->options
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRepo::CloneWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->out != NULL) {
    to = GitRepo::New((void *)baton->out);
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
  free((void *)baton->url);
  free((void *)baton->local_path);
  delete baton;
}

/**
 * @param {String} name
 * @return {Remote} out
 */
NAN_METHOD(GitRepo::GetRemote) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError("String name is required.");
  }

  git_remote * out = 0;
  const char * from_name;
            String::Utf8Value name(args[0]->ToString());
      from_name = strdup(*name);
      
  int result = git_remote_load(
    &out
    , ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue()
    , from_name
  );
  free((void *)from_name);
  if (result != GIT_OK) {
    if (giterr_last()) {
      return NanThrowError(giterr_last()->message);
    } else {
      return NanThrowError("Unknown Error");
    }
  }

  Handle<Value> to;
    if (out != NULL) {
    to = GitRemote::New((void *)out);
  } else {
    to = NanNull();
  }
  NanReturnValue(to);
}

Persistent<Function> GitRepo::constructor_template;

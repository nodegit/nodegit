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


#include "../include/functions/copy.h"

/**
 * @param {String} path
 * @param {Repository} callback
 */
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
    const char * from_path;
            String::Utf8Value path(args[0]->ToString());
      from_path = strdup(*path);
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
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRepo::OpenAfterWork(uv_work_t *req) {
  HandleScope scope;
  OpenBaton *baton = static_cast<OpenBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->out != NULL) {
    to = GitRepo::New((void *)baton->out);
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
  baton->pathReference.Dispose();
  baton->callback.Dispose();
  free((void *)baton->path);
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {String} path
 * @param {Boolean} is_bare
 * @param {Repository} callback
 */
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
    const char * from_path;
            String::Utf8Value path(args[0]->ToString());
      from_path = strdup(*path);
          baton->path = from_path;
    baton->is_bareReference = Persistent<Value>::New(args[1]);
    unsigned from_is_bare;
            from_is_bare = (unsigned)   args[1]->ToBoolean()->Value();
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
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRepo::InitAfterWork(uv_work_t *req) {
  HandleScope scope;
  InitBaton *baton = static_cast<InitBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->out != NULL) {
    to = GitRepo::New((void *)baton->out);
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
  baton->pathReference.Dispose();
  baton->is_bareReference.Dispose();
  baton->callback.Dispose();
  free((void *)baton->path);
  delete baton;
}

/**
 * @return {String} result
 */
Handle<Value> GitRepo::Path(const Arguments& args) {
  HandleScope scope;
  

  const char * result = git_repository_path(
    ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = String::New(result);
  return scope.Close(to);
}

/**
 * @return {String} result
 */
Handle<Value> GitRepo::Workdir(const Arguments& args) {
  HandleScope scope;
  

  const char * result = git_repository_workdir(
    ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = String::New(result);
  return scope.Close(to);
}

/**
 * @return {Odb} out
 */
Handle<Value> GitRepo::Odb(const Arguments& args) {
  HandleScope scope;
  
  git_odb * out = 0;

  int result = git_repository_odb(
    &out
    , ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue()
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
    to = GitOdb::New((void *)out);
  } else {
    to = Null();
  }
  return scope.Close(to);
}

#include "../include/functions/copy.h"

/**
 * @param {Index} callback
 */
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
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRepo::openIndexAfterWork(uv_work_t *req) {
  HandleScope scope;
  openIndexBaton *baton = static_cast<openIndexBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->out != NULL) {
    to = GitIndex::New((void *)baton->out);
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
  baton->repoReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {Oid} id
 * @param {Blob} callback
 */
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
    const git_oid * from_id;
            from_id = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject())->GetValue();
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
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRepo::GetBlobAfterWork(uv_work_t *req) {
  HandleScope scope;
  GetBlobBaton *baton = static_cast<GetBlobBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->blob != NULL) {
    to = GitBlob::New((void *)baton->blob);
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
  baton->repoReference.Dispose();
  baton->idReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {Oid} id
 * @param {Commit} callback
 */
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
    const git_oid * from_id;
            from_id = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject())->GetValue();
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
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRepo::GetCommitAfterWork(uv_work_t *req) {
  HandleScope scope;
  GetCommitBaton *baton = static_cast<GetCommitBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->commit != NULL) {
    to = GitCommit::New((void *)baton->commit);
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
  baton->repoReference.Dispose();
  baton->idReference.Dispose();
  baton->callback.Dispose();
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
Handle<Value> GitRepo::CreateCommit(const Arguments& args) {
  HandleScope scope;
      if (args.Length() == 1 || !args[1]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Signature author is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Signature committer is required.")));
  }
  if (args.Length() == 4 || !args[4]->IsString()) {
    return ThrowException(Exception::Error(String::New("String message is required.")));
  }
  if (args.Length() == 5 || !args[5]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Tree tree is required.")));
  }
  if (args.Length() == 6 || !args[6]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number parent_count is required.")));
  }
  if (args.Length() == 7 || !args[7]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Array parents is required.")));
  }

  if (args.Length() == 8 || !args[8]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  CreateCommitBaton* baton = new CreateCommitBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->id = (git_oid *)malloc(sizeof(git_oid ));
  baton->repoReference = Persistent<Value>::New(args.This());
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue();
  baton->update_refReference = Persistent<Value>::New(args[0]);
    const char * from_update_ref;
      if (args[0]->IsString()) {
            String::Utf8Value update_ref(args[0]->ToString());
      from_update_ref = strdup(*update_ref);
          } else {
      from_update_ref = 0;
    }
      baton->update_ref = from_update_ref;
    baton->authorReference = Persistent<Value>::New(args[1]);
    const git_signature * from_author;
            from_author = ObjectWrap::Unwrap<GitSignature>(args[1]->ToObject())->GetValue();
          baton->author = from_author;
    baton->committerReference = Persistent<Value>::New(args[2]);
    const git_signature * from_committer;
            from_committer = ObjectWrap::Unwrap<GitSignature>(args[2]->ToObject())->GetValue();
          baton->committer = from_committer;
    baton->message_encodingReference = Persistent<Value>::New(args[3]);
    const char * from_message_encoding;
      if (args[3]->IsString()) {
            String::Utf8Value message_encoding(args[3]->ToString());
      from_message_encoding = strdup(*message_encoding);
          } else {
      from_message_encoding = 0;
    }
      baton->message_encoding = from_message_encoding;
    baton->messageReference = Persistent<Value>::New(args[4]);
    const char * from_message;
            String::Utf8Value message(args[4]->ToString());
      from_message = strdup(*message);
          baton->message = from_message;
    baton->treeReference = Persistent<Value>::New(args[5]);
    const git_tree * from_tree;
            from_tree = ObjectWrap::Unwrap<GitTree>(args[5]->ToObject())->GetValue();
          baton->tree = from_tree;
    baton->parent_countReference = Persistent<Value>::New(args[6]);
    int from_parent_count;
            from_parent_count = (int)   args[6]->ToInt32()->Value();
          baton->parent_count = from_parent_count;
    baton->parentsReference = Persistent<Value>::New(args[7]);
    const git_commit ** from_parents;
            Array *tmp_parents = Array::Cast(*args[7]);
      from_parents = (const git_commit **)malloc(tmp_parents->Length() * sizeof(const git_commit *));
      for (unsigned int i = 0; i < tmp_parents->Length(); i++) {
    
        from_parents[i] = ObjectWrap::Unwrap<GitCommit>(tmp_parents->Get(Number::New(static_cast<double>(i)))->ToObject())->GetValue();
      }
          baton->parents = from_parents;
    baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[8]));

  uv_queue_work(uv_default_loop(), &baton->request, CreateCommitWork, (uv_after_work_cb)CreateCommitAfterWork);

  return Undefined();
}

void GitRepo::CreateCommitWork(uv_work_t *req) {
  CreateCommitBaton *baton = static_cast<CreateCommitBaton *>(req->data);
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

void GitRepo::CreateCommitAfterWork(uv_work_t *req) {
  HandleScope scope;
  CreateCommitBaton *baton = static_cast<CreateCommitBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->id != NULL) {
    to = GitOid::New((void *)baton->id);
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
        free(baton->id);
      }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  baton->repoReference.Dispose();
  baton->update_refReference.Dispose();
  baton->authorReference.Dispose();
  baton->committerReference.Dispose();
  baton->message_encodingReference.Dispose();
  baton->messageReference.Dispose();
  baton->treeReference.Dispose();
  baton->parent_countReference.Dispose();
  baton->parentsReference.Dispose();
  baton->callback.Dispose();
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
    const git_oid * from_id;
            from_id = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject())->GetValue();
          baton->id = from_id;
    baton->typeReference = Persistent<Value>::New(args[1]);
    git_otype from_type;
            from_type = (git_otype)   args[1]->ToInt32()->Value();
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
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRepo::GetObjectAfterWork(uv_work_t *req) {
  HandleScope scope;
  GetObjectBaton *baton = static_cast<GetObjectBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->object != NULL) {
    to = GitObject::New((void *)baton->object);
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
  baton->repoReference.Dispose();
  baton->idReference.Dispose();
  baton->typeReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {String} name
 * @param {Reference} callback
 */
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
    const char * from_name;
            String::Utf8Value name(args[0]->ToString());
      from_name = strdup(*name);
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
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRepo::GetReferenceAfterWork(uv_work_t *req) {
  HandleScope scope;
  GetReferenceBaton *baton = static_cast<GetReferenceBaton *>(req->data);

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
  baton->repoReference.Dispose();
  baton->nameReference.Dispose();
  baton->callback.Dispose();
  free((void *)baton->name);
  delete baton;
}

/**
 * @param {String} name
 * @param {String} target
 * @param {Number} force
 * @return {Reference} out
 */
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
 * @param {String} name
 * @param {Oid} id
 * @param {Number} force
 * @return {Reference} out
 */
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

#include "../include/functions/copy.h"

/**
 * @param {String} name
 * @param {String} url
 * @param {Remote} callback
 */
Handle<Value> GitRepo::AddRemote(const Arguments& args) {
  HandleScope scope;
      if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String name is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsString()) {
    return ThrowException(Exception::Error(String::New("String url is required.")));
  }

  if (args.Length() == 2 || !args[2]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  AddRemoteBaton* baton = new AddRemoteBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->repoReference = Persistent<Value>::New(args.This());
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue();
  baton->nameReference = Persistent<Value>::New(args[0]);
    const char * from_name;
            String::Utf8Value name(args[0]->ToString());
      from_name = strdup(*name);
          baton->name = from_name;
    baton->urlReference = Persistent<Value>::New(args[1]);
    const char * from_url;
            String::Utf8Value url(args[1]->ToString());
      from_url = strdup(*url);
          baton->url = from_url;
    baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[2]));

  uv_queue_work(uv_default_loop(), &baton->request, AddRemoteWork, (uv_after_work_cb)AddRemoteAfterWork);

  return Undefined();
}

void GitRepo::AddRemoteWork(uv_work_t *req) {
  AddRemoteBaton *baton = static_cast<AddRemoteBaton *>(req->data);
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

void GitRepo::AddRemoteAfterWork(uv_work_t *req) {
  HandleScope scope;
  AddRemoteBaton *baton = static_cast<AddRemoteBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->out != NULL) {
    to = GitRemote::New((void *)baton->out);
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
  baton->repoReference.Dispose();
  baton->nameReference.Dispose();
  baton->urlReference.Dispose();
  baton->callback.Dispose();
  free((void *)baton->name);
  free((void *)baton->url);
  delete baton;
}

/**
 * @return {RevWalk} out
 */
Handle<Value> GitRepo::CreateRevWalk(const Arguments& args) {
  HandleScope scope;
  
  git_revwalk * out = 0;

  int result = git_revwalk_new(
    &out
    , ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue()
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
    to = GitRevWalk::New((void *)out);
  } else {
    to = Null();
  }
  return scope.Close(to);
}

/**
 * @param {String} name
 * @return {Submodule} submodule
 */
Handle<Value> GitRepo::GetSubmodule(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String name is required.")));
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
      return ThrowException(Exception::Error(String::New(giterr_last()->message)));
    } else {
      return ThrowException(Exception::Error(String::New("Unkown Error")));
    }
  }

  Handle<Value> to;
    if (submodule != NULL) {
    to = GitSubmodule::New((void *)submodule);
  } else {
    to = Null();
  }
  return scope.Close(to);
}

/**
 * @param {String} url
 * @param {String} path
 * @param {Number} use_gitlink
 * @return {Submodule} submodule
 */
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
      return ThrowException(Exception::Error(String::New(giterr_last()->message)));
    } else {
      return ThrowException(Exception::Error(String::New("Unkown Error")));
    }
  }

  Handle<Value> to;
    if (submodule != NULL) {
    to = GitSubmodule::New((void *)submodule);
  } else {
    to = Null();
  }
  return scope.Close(to);
}

#include "../include/functions/copy.h"

/**
 * @param {Oid} id
 * @param {Tag} callback
 */
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
    const git_oid * from_id;
            from_id = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject())->GetValue();
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
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRepo::GetTagAfterWork(uv_work_t *req) {
  HandleScope scope;
  GetTagBaton *baton = static_cast<GetTagBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->out != NULL) {
    to = GitTag::New((void *)baton->out);
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
  baton->repoReference.Dispose();
  baton->idReference.Dispose();
  baton->callback.Dispose();
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
    const char * from_tag_name;
            String::Utf8Value tag_name(args[0]->ToString());
      from_tag_name = strdup(*tag_name);
          baton->tag_name = from_tag_name;
    baton->targetReference = Persistent<Value>::New(args[1]);
    const git_object * from_target;
            from_target = ObjectWrap::Unwrap<GitObject>(args[1]->ToObject())->GetValue();
          baton->target = from_target;
    baton->taggerReference = Persistent<Value>::New(args[2]);
    const git_signature * from_tagger;
            from_tagger = ObjectWrap::Unwrap<GitSignature>(args[2]->ToObject())->GetValue();
          baton->tagger = from_tagger;
    baton->messageReference = Persistent<Value>::New(args[3]);
    const char * from_message;
            String::Utf8Value message(args[3]->ToString());
      from_message = strdup(*message);
          baton->message = from_message;
    baton->forceReference = Persistent<Value>::New(args[4]);
    int from_force;
            from_force = (int)   args[4]->ToInt32()->Value();
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
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRepo::CreateTagAfterWork(uv_work_t *req) {
  HandleScope scope;
  CreateTagBaton *baton = static_cast<CreateTagBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->oid != NULL) {
    to = GitOid::New((void *)baton->oid);
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
        free(baton->oid);
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

#include "../include/functions/copy.h"

/**
 * @param {String} tag_name
 * @param {Object} target
 * @param {Number} force
 * @param {Oid} callback
 */
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
    const char * from_tag_name;
            String::Utf8Value tag_name(args[0]->ToString());
      from_tag_name = strdup(*tag_name);
          baton->tag_name = from_tag_name;
    baton->targetReference = Persistent<Value>::New(args[1]);
    const git_object * from_target;
            from_target = ObjectWrap::Unwrap<GitObject>(args[1]->ToObject())->GetValue();
          baton->target = from_target;
    baton->forceReference = Persistent<Value>::New(args[2]);
    int from_force;
            from_force = (int)   args[2]->ToInt32()->Value();
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
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRepo::CreateLightweightTagAfterWork(uv_work_t *req) {
  HandleScope scope;
  CreateLightweightTagBaton *baton = static_cast<CreateLightweightTagBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->oid != NULL) {
    to = GitOid::New((void *)baton->oid);
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
        free(baton->oid);
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

#include "../include/functions/copy.h"

/**
 * @param {Oid} id
 * @param {Tree} callback
 */
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
    const git_oid * from_id;
            from_id = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject())->GetValue();
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
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRepo::GetTreeAfterWork(uv_work_t *req) {
  HandleScope scope;
  GetTreeBaton *baton = static_cast<GetTreeBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->out != NULL) {
    to = GitTree::New((void *)baton->out);
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
  baton->repoReference.Dispose();
  baton->idReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

#include "../include/functions/copy.h"

/**
 */
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
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
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
  baton->repoReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {String} tag_name
 */
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
    const char * from_tag_name;
            String::Utf8Value tag_name(args[0]->ToString());
      from_tag_name = strdup(*tag_name);
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
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
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
  baton->repoReference.Dispose();
  baton->tag_nameReference.Dispose();
  baton->callback.Dispose();
  free((void *)baton->tag_name);
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {Number} list_flags
 * @param {Array} callback
 */
Handle<Value> GitRepo::GetReferences(const Arguments& args) {
  HandleScope scope;
    
  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  GetReferencesBaton* baton = new GetReferencesBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->array = (git_strarray *)malloc(sizeof(git_strarray ));
  baton->repoReference = Persistent<Value>::New(args.This());
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue();
  baton->list_flagsReference = Persistent<Value>::New(args[0]);
    unsigned int from_list_flags;
      if (args[0]->IsUint32()) {
            from_list_flags = (unsigned int)   args[0]->ToUint32()->Value();
          } else {
      from_list_flags = 0;
    }
      baton->list_flags = from_list_flags;
    baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, GetReferencesWork, (uv_after_work_cb)GetReferencesAfterWork);

  return Undefined();
}

void GitRepo::GetReferencesWork(uv_work_t *req) {
  GetReferencesBaton *baton = static_cast<GetReferencesBaton *>(req->data);
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

void GitRepo::GetReferencesAfterWork(uv_work_t *req) {
  HandleScope scope;
  GetReferencesBaton *baton = static_cast<GetReferencesBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
  
  Local<Array> tmpArray = Array::New(baton->array->count);
  for (unsigned int i = 0; i < baton->array->count; i++) {
    tmpArray->Set(Number::New(i), String::New(baton->array->strings[i]));
  }
  to = tmpArray;
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
        free(baton->array);
      }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  baton->repoReference.Dispose();
  baton->list_flagsReference.Dispose();
  baton->callback.Dispose();

  git_strarray_free(baton->array);
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {Buffer} buffer
 * @param {Number} len
 * @param {Oid} callback
 */
Handle<Value> GitRepo::CreateBlobFromBuffer(const Arguments& args) {
  HandleScope scope;
      if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Buffer buffer is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsNumber()) {
    return ThrowException(Exception::Error(String::New("Number len is required.")));
  }

  if (args.Length() == 2 || !args[2]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  CreateBlobFromBufferBaton* baton = new CreateBlobFromBufferBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->oid = (git_oid *)malloc(sizeof(git_oid ));
  baton->repoReference = Persistent<Value>::New(args.This());
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue();
  baton->bufferReference = Persistent<Value>::New(args[0]);
    const void * from_buffer;
            from_buffer = Buffer::Data(args[0]->ToObject());
          baton->buffer = from_buffer;
    baton->lenReference = Persistent<Value>::New(args[1]);
    size_t from_len;
            from_len = (size_t)   args[1]->ToNumber()->Value();
          baton->len = from_len;
    baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[2]));

  uv_queue_work(uv_default_loop(), &baton->request, CreateBlobFromBufferWork, (uv_after_work_cb)CreateBlobFromBufferAfterWork);

  return Undefined();
}

void GitRepo::CreateBlobFromBufferWork(uv_work_t *req) {
  CreateBlobFromBufferBaton *baton = static_cast<CreateBlobFromBufferBaton *>(req->data);
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

void GitRepo::CreateBlobFromBufferAfterWork(uv_work_t *req) {
  HandleScope scope;
  CreateBlobFromBufferBaton *baton = static_cast<CreateBlobFromBufferBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->oid != NULL) {
    to = GitOid::New((void *)baton->oid);
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
        free(baton->oid);
      }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  baton->repoReference.Dispose();
  baton->bufferReference.Dispose();
  baton->lenReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {String} path
 * @param {Oid} callback
 */
Handle<Value> GitRepo::CreateBlobFromFile(const Arguments& args) {
  HandleScope scope;
      if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String path is required.")));
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  CreateBlobFromFileBaton* baton = new CreateBlobFromFileBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->id = (git_oid *)malloc(sizeof(git_oid ));
  baton->repoReference = Persistent<Value>::New(args.This());
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue();
  baton->pathReference = Persistent<Value>::New(args[0]);
    const char * from_path;
            String::Utf8Value path(args[0]->ToString());
      from_path = strdup(*path);
          baton->path = from_path;
    baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, CreateBlobFromFileWork, (uv_after_work_cb)CreateBlobFromFileAfterWork);

  return Undefined();
}

void GitRepo::CreateBlobFromFileWork(uv_work_t *req) {
  CreateBlobFromFileBaton *baton = static_cast<CreateBlobFromFileBaton *>(req->data);
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

void GitRepo::CreateBlobFromFileAfterWork(uv_work_t *req) {
  HandleScope scope;
  CreateBlobFromFileBaton *baton = static_cast<CreateBlobFromFileBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->id != NULL) {
    to = GitOid::New((void *)baton->id);
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
        free(baton->id);
      }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  baton->repoReference.Dispose();
  baton->pathReference.Dispose();
  baton->callback.Dispose();
  free((void *)baton->path);
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {Array} callback
 */
Handle<Value> GitRepo::GetRemotes(const Arguments& args) {
  HandleScope scope;
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  GetRemotesBaton* baton = new GetRemotesBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->out = (git_strarray *)malloc(sizeof(git_strarray ));
  baton->repoReference = Persistent<Value>::New(args.This());
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args.This())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, GetRemotesWork, (uv_after_work_cb)GetRemotesAfterWork);

  return Undefined();
}

void GitRepo::GetRemotesWork(uv_work_t *req) {
  GetRemotesBaton *baton = static_cast<GetRemotesBaton *>(req->data);
  int result = git_remote_list(
    baton->out, 
    baton->repo
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRepo::GetRemotesAfterWork(uv_work_t *req) {
  HandleScope scope;
  GetRemotesBaton *baton = static_cast<GetRemotesBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
  
  Local<Array> tmpArray = Array::New(baton->out->count);
  for (unsigned int i = 0; i < baton->out->count; i++) {
    tmpArray->Set(Number::New(i), String::New(baton->out->strings[i]));
  }
  to = tmpArray;
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
        free(baton->out);
      }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  baton->repoReference.Dispose();
  baton->callback.Dispose();

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
Handle<Value> GitRepo::Clone(const Arguments& args) {
  HandleScope scope;
      if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String url is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsString()) {
    return ThrowException(Exception::Error(String::New("String local_path is required.")));
  }

  if (args.Length() == 3 || !args[3]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  CloneBaton* baton = new CloneBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->urlReference = Persistent<Value>::New(args[0]);
    const char * from_url;
            String::Utf8Value url(args[0]->ToString());
      from_url = strdup(*url);
          baton->url = from_url;
    baton->local_pathReference = Persistent<Value>::New(args[1]);
    const char * from_local_path;
            String::Utf8Value local_path(args[1]->ToString());
      from_local_path = strdup(*local_path);
          baton->local_path = from_local_path;
    baton->optionsReference = Persistent<Value>::New(args[2]);
    const git_clone_options * from_options;
      if (args[2]->IsObject()) {
            from_options = ObjectWrap::Unwrap<GitCloneOptions>(args[2]->ToObject())->GetValue();
          } else {
      from_options = 0;
    }
      baton->options = from_options;
    baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[3]));

  uv_queue_work(uv_default_loop(), &baton->request, CloneWork, (uv_after_work_cb)CloneAfterWork);

  return Undefined();
}

void GitRepo::CloneWork(uv_work_t *req) {
  CloneBaton *baton = static_cast<CloneBaton *>(req->data);
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

void GitRepo::CloneAfterWork(uv_work_t *req) {
  HandleScope scope;
  CloneBaton *baton = static_cast<CloneBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->out != NULL) {
    to = GitRepo::New((void *)baton->out);
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
  baton->urlReference.Dispose();
  baton->local_pathReference.Dispose();
  baton->optionsReference.Dispose();
  baton->callback.Dispose();
  free((void *)baton->url);
  free((void *)baton->local_path);
  delete baton;
}

/**
 * @param {String} name
 * @return {Remote} out
 */
Handle<Value> GitRepo::GetRemote(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String name is required.")));
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
      return ThrowException(Exception::Error(String::New(giterr_last()->message)));
    } else {
      return ThrowException(Exception::Error(String::New("Unkown Error")));
    }
  }

  Handle<Value> to;
    if (out != NULL) {
    to = GitRemote::New((void *)out);
  } else {
    to = Null();
  }
  return scope.Close(to);
}

Persistent<Function> GitRepo::constructor_template;

/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/functions/copy.h"

#include "../include/index.h"
#include "../include/oid.h"
#include "../include/repo.h"
#include "../include/tree.h"
#include "../include/diff_list.h"
#include "../include/diff_options.h"
#include "../include/index_entry.h"

using namespace v8;
using namespace node;

GitIndex::GitIndex(git_index *raw) {
  this->raw = raw;
}

GitIndex::~GitIndex() {
  git_index_free(this->raw);
}

void GitIndex::Initialize(Handle<v8::Object> target) {
  NanScope();

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanSymbol("Index"));

  NODE_SET_METHOD(tpl, "open", Open);
  NODE_SET_PROTOTYPE_METHOD(tpl, "read", Read);
  NODE_SET_PROTOTYPE_METHOD(tpl, "write", Write);
  NODE_SET_PROTOTYPE_METHOD(tpl, "readTree", ReadTree);
  NODE_SET_PROTOTYPE_METHOD(tpl, "writeTree", WriteTree);
  NODE_SET_PROTOTYPE_METHOD(tpl, "size", Size);
  NODE_SET_PROTOTYPE_METHOD(tpl, "clear", Clear);
  NODE_SET_PROTOTYPE_METHOD(tpl, "entry", Entry);
  NODE_SET_PROTOTYPE_METHOD(tpl, "remove", Remove);
  NODE_SET_PROTOTYPE_METHOD(tpl, "removeDirectory", RemoveDirectory);
  NODE_SET_PROTOTYPE_METHOD(tpl, "addByPath", AddBypath);
  NODE_SET_PROTOTYPE_METHOD(tpl, "removeByPath", RemoveBypath);
  NODE_SET_PROTOTYPE_METHOD(tpl, "find", Find);
  NODE_SET_PROTOTYPE_METHOD(tpl, "conflictRemove", ConflictRemove);
  NODE_SET_PROTOTYPE_METHOD(tpl, "conflictCleanup", ConflictCleanup);
  NODE_SET_PROTOTYPE_METHOD(tpl, "hasConflicts", HasConflicts);
  NODE_SET_METHOD(tpl, "indexToWorkdir", IndexToWorkdir);

  NanAssignPersistent(FunctionTemplate, constructor_template, tpl);
  target->Set(String::NewSymbol("Index"), tpl->GetFunction());
}

NAN_METHOD(GitIndex::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError(String::New("git_index is required."));
  }

  GitIndex* object = new GitIndex((git_index *) External::Cast(*args[0])->Value());
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitIndex::New(void *raw) {
  NanScope();
  Handle<Value> argv[1] = { External::New((void *)raw) };
  Local<Object> instance;
  Local<FunctionTemplate> constructorHandle = NanPersistentToLocal(constructor_template);
  instance = constructorHandle->GetFunction()->NewInstance(1, argv);
  return scope.Close(instance);
}

git_index *GitIndex::GetValue() {
  return this->raw;
}


#include "../include/functions/copy.h"

/**
 * @param {String} index_path
 * @param {Index} callback
 */
NAN_METHOD(GitIndex::Open) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError(String::New("String index_path is required."));
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError(String::New("Callback is required and must be a Function."));
  }

  OpenBaton* baton = new OpenBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  NanAssignPersistent(Value, baton->index_pathReference, args[0]);
  const char * from_index_path;
  String::Utf8Value index_path(args[0]->ToString());
  from_index_path = strdup(*index_path);
  baton->index_path = from_index_path;
  NanAssignPersistent(Function, baton->callback, Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, OpenWork, (uv_after_work_cb)OpenAfterWork);

  NanReturnUndefined();
}

void GitIndex::OpenWork(uv_work_t *req) {
  OpenBaton *baton = static_cast<OpenBaton *>(req->data);
  int result = git_index_open(
    &baton->out, 
    baton->index_path
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitIndex::OpenAfterWork(uv_work_t *req) {
  NanScope();
  OpenBaton *baton = static_cast<OpenBaton *>(req->data);

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
  baton->index_pathReference.Dispose();
  baton->callback.Dispose();
  free((void *)baton->index_path);
  delete baton;
}

#include "../include/functions/copy.h"

/**
 */
NAN_METHOD(GitIndex::Read) {
  NanScope();
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return NanThrowError(String::New("Callback is required and must be a Function."));
  }

  ReadBaton* baton = new ReadBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  NanAssignPersistent(Value, baton->indexReference, args.This());
  baton->index = ObjectWrap::Unwrap<GitIndex>(args.This())->GetValue();
  NanAssignPersistent(Function, baton->callback, Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, ReadWork, (uv_after_work_cb)ReadAfterWork);

  NanReturnUndefined();
}

void GitIndex::ReadWork(uv_work_t *req) {
  ReadBaton *baton = static_cast<ReadBaton *>(req->data);
  int result = git_index_read(
    baton->index
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitIndex::ReadAfterWork(uv_work_t *req) {
  NanScope();
  ReadBaton *baton = static_cast<ReadBaton *>(req->data);

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
  baton->indexReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

#include "../include/functions/copy.h"

/**
 */
NAN_METHOD(GitIndex::Write) {
  NanScope();
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return NanThrowError(String::New("Callback is required and must be a Function."));
  }

  WriteBaton* baton = new WriteBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  NanAssignPersistent(Value, baton->indexReference, args.This());
  baton->index = ObjectWrap::Unwrap<GitIndex>(args.This())->GetValue();
  NanAssignPersistent(Function, baton->callback, Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, WriteWork, (uv_after_work_cb)WriteAfterWork);

  NanReturnUndefined();
}

void GitIndex::WriteWork(uv_work_t *req) {
  WriteBaton *baton = static_cast<WriteBaton *>(req->data);
  int result = git_index_write(
    baton->index
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitIndex::WriteAfterWork(uv_work_t *req) {
  NanScope();
  WriteBaton *baton = static_cast<WriteBaton *>(req->data);

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
  baton->indexReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {Tree} tree
 */
NAN_METHOD(GitIndex::ReadTree) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsObject()) {
    return NanThrowError(String::New("Tree tree is required."));
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError(String::New("Callback is required and must be a Function."));
  }

  ReadTreeBaton* baton = new ReadTreeBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  NanAssignPersistent(Value, baton->indexReference, args.This());
  baton->index = ObjectWrap::Unwrap<GitIndex>(args.This())->GetValue();
  NanAssignPersistent(Value, baton->treeReference, args[0]);
  const git_tree * from_tree;
  from_tree = ObjectWrap::Unwrap<GitTree>(args[0]->ToObject())->GetValue();
  baton->tree = from_tree;
  NanAssignPersistent(Function, baton->callback, Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, ReadTreeWork, (uv_after_work_cb)ReadTreeAfterWork);

  NanReturnUndefined();
}

void GitIndex::ReadTreeWork(uv_work_t *req) {
  ReadTreeBaton *baton = static_cast<ReadTreeBaton *>(req->data);
  int result = git_index_read_tree(
    baton->index, 
    baton->tree
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitIndex::ReadTreeAfterWork(uv_work_t *req) {
  NanScope();
  ReadTreeBaton *baton = static_cast<ReadTreeBaton *>(req->data);

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
  baton->indexReference.Dispose();
  baton->treeReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {Oid} callback
 */
NAN_METHOD(GitIndex::WriteTree) {
  NanScope();
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return NanThrowError(String::New("Callback is required and must be a Function."));
  }

  WriteTreeBaton* baton = new WriteTreeBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->out = (git_oid *)malloc(sizeof(git_oid ));
  NanAssignPersistent(Value, baton->indexReference, args.This());
  baton->index = ObjectWrap::Unwrap<GitIndex>(args.This())->GetValue();
  NanAssignPersistent(Function, baton->callback, Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, WriteTreeWork, (uv_after_work_cb)WriteTreeAfterWork);

  NanReturnUndefined();
}

void GitIndex::WriteTreeWork(uv_work_t *req) {
  WriteTreeBaton *baton = static_cast<WriteTreeBaton *>(req->data);
  int result = git_index_write_tree(
    baton->out, 
    baton->index
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitIndex::WriteTreeAfterWork(uv_work_t *req) {
  NanScope();
  WriteTreeBaton *baton = static_cast<WriteTreeBaton *>(req->data);

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
  baton->indexReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

/**
 * @return {Number} result
 */
NAN_METHOD(GitIndex::Size) {
  NanScope();
  

  size_t result = git_index_entrycount(
    ObjectWrap::Unwrap<GitIndex>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = Uint32::New(result);
  NanReturnValue(to);
}

/**
 */
NAN_METHOD(GitIndex::Clear) {
  NanScope();
  

  git_index_clear(
    ObjectWrap::Unwrap<GitIndex>(args.This())->GetValue()
  );

  NanReturnUndefined();
}

/**
 * @param {Number} n
 * @return {IndexEntry} result
 */
NAN_METHOD(GitIndex::Entry) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsUint32()) {
    return NanThrowError(String::New("Number n is required."));
  }

  size_t from_n;
            from_n = (size_t) args[0]->ToUint32()->Value();
      
  const git_index_entry * result = git_index_get_byindex(
    ObjectWrap::Unwrap<GitIndex>(args.This())->GetValue()
    , from_n
  );

  Handle<Value> to;
    if (result != NULL) {
    result = (const git_index_entry * )git_index_entry_dup(result);
  }
  if (result != NULL) {
    to = GitIndexEntry::New((void *)result);
  } else {
    to = Null();
  }
  NanReturnValue(to);
}

/**
 * @param {String} path
 * @param {Number} stage
 */
NAN_METHOD(GitIndex::Remove) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError(String::New("String path is required."));
  }
  if (args.Length() == 1 || !args[1]->IsInt32()) {
    return NanThrowError(String::New("Number stage is required."));
  }

  const char * from_path;
            String::Utf8Value path(args[0]->ToString());
      from_path = strdup(*path);
        int from_stage;
            from_stage = (int) args[1]->ToInt32()->Value();
      
  int result = git_index_remove(
    ObjectWrap::Unwrap<GitIndex>(args.This())->GetValue()
    , from_path
    , from_stage
  );
  free((void *)from_path);
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
 * @param {String} dir
 * @param {Number} stage
 */
NAN_METHOD(GitIndex::RemoveDirectory) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError(String::New("String dir is required."));
  }
  if (args.Length() == 1 || !args[1]->IsInt32()) {
    return NanThrowError(String::New("Number stage is required."));
  }

  const char * from_dir;
            String::Utf8Value dir(args[0]->ToString());
      from_dir = strdup(*dir);
        int from_stage;
            from_stage = (int) args[1]->ToInt32()->Value();
      
  int result = git_index_remove_directory(
    ObjectWrap::Unwrap<GitIndex>(args.This())->GetValue()
    , from_dir
    , from_stage
  );
  free((void *)from_dir);
  if (result != GIT_OK) {
    if (giterr_last()) {
      return NanThrowError(String::New(giterr_last()->message));
    } else {
      return NanThrowError(String::New("Unkown Error"));
    }
  }

  NanReturnUndefined();
}

#include "../include/functions/copy.h"

/**
 * @param {String} path
 */
NAN_METHOD(GitIndex::AddBypath) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError(String::New("String path is required."));
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError(String::New("Callback is required and must be a Function."));
  }

  AddBypathBaton* baton = new AddBypathBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  NanAssignPersistent(Value, baton->indexReference, args.This());
  baton->index = ObjectWrap::Unwrap<GitIndex>(args.This())->GetValue();
  NanAssignPersistent(Value, baton->pathReference, args[0]);
  const char * from_path;
  String::Utf8Value path(args[0]->ToString());
  from_path = strdup(*path);
  baton->path = from_path;
  NanAssignPersistent(Function, baton->callback, Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, AddBypathWork, (uv_after_work_cb)AddBypathAfterWork);

  NanReturnUndefined();
}

void GitIndex::AddBypathWork(uv_work_t *req) {
  AddBypathBaton *baton = static_cast<AddBypathBaton *>(req->data);
  int result = git_index_add_bypath(
    baton->index, 
    baton->path
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitIndex::AddBypathAfterWork(uv_work_t *req) {
  NanScope();
  AddBypathBaton *baton = static_cast<AddBypathBaton *>(req->data);

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
  baton->indexReference.Dispose();
  baton->pathReference.Dispose();
  baton->callback.Dispose();
  free((void *)baton->path);
  delete baton;
}

/**
 * @param {String} path
 */
NAN_METHOD(GitIndex::RemoveBypath) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError(String::New("String path is required."));
  }

  const char * from_path;
            String::Utf8Value path(args[0]->ToString());
      from_path = strdup(*path);
      
  int result = git_index_remove_bypath(
    ObjectWrap::Unwrap<GitIndex>(args.This())->GetValue()
    , from_path
  );
  free((void *)from_path);
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
 * @param {String} path
 * @return {Number} at_pos
 */
NAN_METHOD(GitIndex::Find) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError(String::New("String path is required."));
  }

  size_t at_pos = 0;
  const char * from_path;
            String::Utf8Value path(args[0]->ToString());
      from_path = strdup(*path);
      
  int result = git_index_find(
    &at_pos
    , ObjectWrap::Unwrap<GitIndex>(args.This())->GetValue()
    , from_path
  );
  free((void *)from_path);

  Handle<Value> to;
    to = Uint32::New(at_pos);
  NanReturnValue(to);
}

/**
 * @param {String} path
 */
NAN_METHOD(GitIndex::ConflictRemove) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError(String::New("String path is required."));
  }

  const char * from_path;
            String::Utf8Value path(args[0]->ToString());
      from_path = strdup(*path);
      
  int result = git_index_conflict_remove(
    ObjectWrap::Unwrap<GitIndex>(args.This())->GetValue()
    , from_path
  );
  free((void *)from_path);
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
NAN_METHOD(GitIndex::ConflictCleanup) {
  NanScope();
  

  git_index_conflict_cleanup(
    ObjectWrap::Unwrap<GitIndex>(args.This())->GetValue()
  );

  NanReturnUndefined();
}

/**
 * @return {Number} result
 */
NAN_METHOD(GitIndex::HasConflicts) {
  NanScope();
  

  int result = git_index_has_conflicts(
    ObjectWrap::Unwrap<GitIndex>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = Int32::New(result);
  NanReturnValue(to);
}

#include "../include/functions/copy.h"

/**
 * @param {Repository} repo
 * @param {Index} index
 * @param {DiffOptions} opts
 * @param {DiffList} callback
 */
NAN_METHOD(GitIndex::IndexToWorkdir) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsObject()) {
    return NanThrowError(String::New("Repository repo is required."));
  }

  if (args.Length() == 3 || !args[3]->IsFunction()) {
    return NanThrowError(String::New("Callback is required and must be a Function."));
  }

  IndexToWorkdirBaton* baton = new IndexToWorkdirBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  NanAssignPersistent(Value, baton->repoReference, args[0]);
  git_repository * from_repo;
  from_repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
  baton->repo = from_repo;
  NanAssignPersistent(Value, baton->indexReference, args[1]);
  git_index * from_index;
  if (args[1]->IsObject()) {
    from_index = ObjectWrap::Unwrap<GitIndex>(args[1]->ToObject())->GetValue();
  } else {
    from_index = 0;
  }
  baton->index = from_index;
  NanAssignPersistent(Value, baton->optsReference, args[2]);
  const git_diff_options * from_opts;
  if (args[2]->IsObject()) {
    from_opts = ObjectWrap::Unwrap<GitDiffOptions>(args[2]->ToObject())->GetValue();
  } else {
    from_opts = 0;
  }
  baton->opts = from_opts;
  NanAssignPersistent(Function, baton->callback, Local<Function>::Cast(args[3]));

  uv_queue_work(uv_default_loop(), &baton->request, IndexToWorkdirWork, (uv_after_work_cb)IndexToWorkdirAfterWork);

  NanReturnUndefined();
}

void GitIndex::IndexToWorkdirWork(uv_work_t *req) {
  IndexToWorkdirBaton *baton = static_cast<IndexToWorkdirBaton *>(req->data);
  int result = git_diff_index_to_workdir(
    &baton->diff, 
    baton->repo, 
    baton->index, 
    baton->opts
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitIndex::IndexToWorkdirAfterWork(uv_work_t *req) {
  NanScope();
  IndexToWorkdirBaton *baton = static_cast<IndexToWorkdirBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->diff != NULL) {
    to = GitDiffList::New((void *)baton->diff);
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
  baton->repoReference.Dispose();
  baton->indexReference.Dispose();
  baton->optsReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

Persistent<FunctionTemplate> GitIndex::constructor_template;

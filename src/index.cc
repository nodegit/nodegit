/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <nan.h>
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

  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanNew<String>("Index"));

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


  Local<Function> _constructor_template = tpl->GetFunction();
  NanAssignPersistent(constructor_template, _constructor_template);
  target->Set(NanNew<String>("Index"), _constructor_template);
}

NAN_METHOD(GitIndex::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError("git_index is required.");
  }
  GitIndex* object = new GitIndex(static_cast<git_index *>(Handle<External>::Cast(args[0])->Value()));
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitIndex::New(void *raw) {
  NanEscapableScope();
  Handle<Value> argv[1] = { NanNew<External>((void *)raw) };
  return NanEscapeScope(NanNew<Function>(GitIndex::constructor_template)->NewInstance(1, argv));
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
    return NanThrowError("String index_path is required.");
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  OpenBaton* baton = new OpenBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
    const char * from_index_path;
            String::Utf8Value index_path(args[0]->ToString());
      from_index_path = strdup(*index_path);
          baton->index_path = from_index_path;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[1]));
  OpenWorker *worker = new OpenWorker(baton, callback);
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("index_path", args[0]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitIndex::OpenWorker::Execute() {
  int result = git_index_open(
    &baton->out, 
    baton->index_path
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitIndex::OpenWorker::HandleOKCallback() {
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
  free((void *)baton->index_path);
  delete baton;
}

#include "../include/functions/copy.h"

/**
 */
NAN_METHOD(GitIndex::Read) {
  NanScope();
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  ReadBaton* baton = new ReadBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->index = ObjectWrap::Unwrap<GitIndex>(args.This())->GetValue();

  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[0]));
  ReadWorker *worker = new ReadWorker(baton, callback);
  worker->SaveToPersistent("index", args.This());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitIndex::ReadWorker::Execute() {
  int result = git_index_read(
    baton->index
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitIndex::ReadWorker::HandleOKCallback() {
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
NAN_METHOD(GitIndex::Write) {
  NanScope();
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  WriteBaton* baton = new WriteBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->index = ObjectWrap::Unwrap<GitIndex>(args.This())->GetValue();

  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[0]));
  WriteWorker *worker = new WriteWorker(baton, callback);
  worker->SaveToPersistent("index", args.This());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitIndex::WriteWorker::Execute() {
  int result = git_index_write(
    baton->index
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitIndex::WriteWorker::HandleOKCallback() {
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
 * @param {Tree} tree
 */
NAN_METHOD(GitIndex::ReadTree) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsObject()) {
    return NanThrowError("Tree tree is required.");
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  ReadTreeBaton* baton = new ReadTreeBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->index = ObjectWrap::Unwrap<GitIndex>(args.This())->GetValue();
    const git_tree * from_tree;
            from_tree = ObjectWrap::Unwrap<GitTree>(args[0]->ToObject())->GetValue();
          baton->tree = from_tree;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[1]));
  ReadTreeWorker *worker = new ReadTreeWorker(baton, callback);
  worker->SaveToPersistent("index", args.This());
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("tree", args[0]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitIndex::ReadTreeWorker::Execute() {
  int result = git_index_read_tree(
    baton->index, 
    baton->tree
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitIndex::ReadTreeWorker::HandleOKCallback() {
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
 * @param {Oid} callback
 */
NAN_METHOD(GitIndex::WriteTree) {
  NanScope();
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  WriteTreeBaton* baton = new WriteTreeBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->out = (git_oid *)malloc(sizeof(git_oid ));
  baton->index = ObjectWrap::Unwrap<GitIndex>(args.This())->GetValue();

  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[0]));
  WriteTreeWorker *worker = new WriteTreeWorker(baton, callback);
  worker->SaveToPersistent("index", args.This());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitIndex::WriteTreeWorker::Execute() {
  int result = git_index_write_tree(
    baton->out, 
    baton->index
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitIndex::WriteTreeWorker::HandleOKCallback() {
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
 * @return {Number} result
 */
NAN_METHOD(GitIndex::Size) {
  NanScope();
  

  size_t result = git_index_entrycount(
    ObjectWrap::Unwrap<GitIndex>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = NanNew<Uint32>((uint32_t)result);
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
    return NanThrowError("Number n is required.");
  }

  size_t from_n;
            from_n = (size_t)   args[0]->ToUint32()->Value();
      
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
    to = NanNull();
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
    return NanThrowError("String path is required.");
  }
  if (args.Length() == 1 || !args[1]->IsInt32()) {
    return NanThrowError("Number stage is required.");
  }

  const char * from_path;
            String::Utf8Value path(args[0]->ToString());
      from_path = strdup(*path);
        int from_stage;
            from_stage = (int)   args[1]->ToInt32()->Value();
      
  int result = git_index_remove(
    ObjectWrap::Unwrap<GitIndex>(args.This())->GetValue()
    , from_path
    , from_stage
  );
  free((void *)from_path);
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
 * @param {String} dir
 * @param {Number} stage
 */
NAN_METHOD(GitIndex::RemoveDirectory) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError("String dir is required.");
  }
  if (args.Length() == 1 || !args[1]->IsInt32()) {
    return NanThrowError("Number stage is required.");
  }

  const char * from_dir;
            String::Utf8Value dir(args[0]->ToString());
      from_dir = strdup(*dir);
        int from_stage;
            from_stage = (int)   args[1]->ToInt32()->Value();
      
  int result = git_index_remove_directory(
    ObjectWrap::Unwrap<GitIndex>(args.This())->GetValue()
    , from_dir
    , from_stage
  );
  free((void *)from_dir);
  if (result != GIT_OK) {
    if (giterr_last()) {
      return NanThrowError(giterr_last()->message);
    } else {
      return NanThrowError("Unknown Error");
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
    return NanThrowError("String path is required.");
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  AddBypathBaton* baton = new AddBypathBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->index = ObjectWrap::Unwrap<GitIndex>(args.This())->GetValue();
    const char * from_path;
            String::Utf8Value path(args[0]->ToString());
      from_path = strdup(*path);
          baton->path = from_path;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[1]));
  AddBypathWorker *worker = new AddBypathWorker(baton, callback);
  worker->SaveToPersistent("index", args.This());
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("path", args[0]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitIndex::AddBypathWorker::Execute() {
  int result = git_index_add_bypath(
    baton->index, 
    baton->path
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitIndex::AddBypathWorker::HandleOKCallback() {
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
  free((void *)baton->path);
  delete baton;
}

/**
 * @param {String} path
 */
NAN_METHOD(GitIndex::RemoveBypath) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError("String path is required.");
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
      return NanThrowError(giterr_last()->message);
    } else {
      return NanThrowError("Unknown Error");
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
    return NanThrowError("String path is required.");
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
    to = NanNew<Uint32>((uint32_t)at_pos);
  NanReturnValue(to);
}

/**
 * @param {String} path
 */
NAN_METHOD(GitIndex::ConflictRemove) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError("String path is required.");
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
      return NanThrowError(giterr_last()->message);
    } else {
      return NanThrowError("Unknown Error");
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
    to = NanNew<Int32>((int32_t)result);
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
    return NanThrowError("Repository repo is required.");
  }

  if (args.Length() == 3 || !args[3]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  IndexToWorkdirBaton* baton = new IndexToWorkdirBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
    git_repository * from_repo;
            from_repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
          baton->repo = from_repo;
      git_index * from_index;
      if (args[1]->IsObject()) {
            from_index = ObjectWrap::Unwrap<GitIndex>(args[1]->ToObject())->GetValue();
          } else {
      from_index = 0;
    }
      baton->index = from_index;
      const git_diff_options * from_opts;
      if (args[2]->IsObject()) {
            from_opts = ObjectWrap::Unwrap<GitDiffOptions>(args[2]->ToObject())->GetValue();
          } else {
      from_opts = 0;
    }
      baton->opts = from_opts;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[3]));
  IndexToWorkdirWorker *worker = new IndexToWorkdirWorker(baton, callback);
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("repo", args[0]->ToObject());
  if (!args[1]->IsUndefined() && !args[1]->IsNull())
    worker->SaveToPersistent("index", args[1]->ToObject());
  if (!args[2]->IsUndefined() && !args[2]->IsNull())
    worker->SaveToPersistent("opts", args[2]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitIndex::IndexToWorkdirWorker::Execute() {
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

void GitIndex::IndexToWorkdirWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->diff != NULL) {
    to = GitDiffList::New((void *)baton->diff);
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

Persistent<Function> GitIndex::constructor_template;

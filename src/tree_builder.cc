/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <nan.h>
#include <string.h>

#include "git2.h"

#include "../include/functions/copy.h"

#include "../include/tree_builder.h"
#include "../include/repo.h"
#include "../include/oid.h"
#include "../include/tree_entry.h"
#include "../include/tree.h"
#include "../include/diff_list.h"
#include "../include/diff_options.h"
#include "../include/index.h"

using namespace v8;
using namespace node;

GitTreeBuilder::GitTreeBuilder(git_treebuilder *raw) {
  this->raw = raw;
}

GitTreeBuilder::~GitTreeBuilder() {
  git_treebuilder_free(this->raw);
}

void GitTreeBuilder::Initialize(Handle<v8::Object> target) {
  NanScope();

  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanNew<String>("TreeBuilder"));

  NODE_SET_METHOD(tpl, "create", Create);
  NODE_SET_PROTOTYPE_METHOD(tpl, "clear", Clear);
  NODE_SET_METHOD(tpl, "size", Size);
  NODE_SET_PROTOTYPE_METHOD(tpl, "get", Get);
  NODE_SET_PROTOTYPE_METHOD(tpl, "insert", Insert);
  NODE_SET_PROTOTYPE_METHOD(tpl, "gitTreebuilderRemove", GitTreebuilderRemove);
  NODE_SET_PROTOTYPE_METHOD(tpl, "write", Write);


  Local<Function> _constructor_template = tpl->GetFunction();
  NanAssignPersistent(constructor_template, _constructor_template);
  target->Set(NanNew<String>("TreeBuilder"), _constructor_template);
}

NAN_METHOD(GitTreeBuilder::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError("git_treebuilder is required.");
  }
  GitTreeBuilder* object = new GitTreeBuilder(static_cast<git_treebuilder *>(Handle<External>::Cast(args[0])->Value()));
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitTreeBuilder::New(void *raw) {
  NanEscapableScope();
  Handle<Value> argv[1] = { NanNew<External>((void *)raw) };
  return NanEscapeScope(NanNew<Function>(GitTreeBuilder::constructor_template)->NewInstance(1, argv));
}

git_treebuilder *GitTreeBuilder::GetValue() {
  return this->raw;
}


/**
 * @param {Tree} source
 * @return {TreeBuilder} out
 */
NAN_METHOD(GitTreeBuilder::Create) {
  NanScope();
  
  git_treebuilder * out = 0;
  const git_tree * from_source;
      if (args[0]->IsObject()) {
            from_source = ObjectWrap::Unwrap<GitTree>(args[0]->ToObject())->GetValue();
          } else {
      from_source = 0;
    }
  
  int result = git_treebuilder_create(
    &out
    , from_source
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
    to = GitTreeBuilder::New((void *)out);
  } else {
    to = NanNull();
  }
  NanReturnValue(to);
}

/**
 * @param {TreeBuilder} bld
 */
NAN_METHOD(GitTreeBuilder::Clear) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return NanThrowError("TreeBuilder bld is required.");
  }

  git_treebuilder * from_bld;
            from_bld = ObjectWrap::Unwrap<GitTreeBuilder>(args[0]->ToObject())->GetValue();
      
  git_treebuilder_clear(
    from_bld
  );

  NanReturnUndefined();
}

/**
 * @return {Number} result
 */
NAN_METHOD(GitTreeBuilder::Size) {
  NanScope();
  

  unsigned int result = git_treebuilder_entrycount(
    ObjectWrap::Unwrap<GitTreeBuilder>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = NanNew<Uint32>((uint32_t)result);
  NanReturnValue(to);
}

/**
 * @param {String} filename
 * @return {TreeEntry} result
 */
NAN_METHOD(GitTreeBuilder::Get) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError("String filename is required.");
  }

  const char * from_filename;
            String::Utf8Value filename(args[0]->ToString());
      from_filename = strdup(*filename);
      
  const git_tree_entry * result = git_treebuilder_get(
    ObjectWrap::Unwrap<GitTreeBuilder>(args.This())->GetValue()
    , from_filename
  );
  free((void *)from_filename);

  Handle<Value> to;
    if (result != NULL) {
    result = (const git_tree_entry * )git_tree_entry_dup(result);
  }
  if (result != NULL) {
    to = GitTreeEntry::New((void *)result);
  } else {
    to = NanNull();
  }
  NanReturnValue(to);
}

/**
 * @param {String} filename
 * @param {Oid} id
 * @param {Number} filemode
 * @return {TreeEntry} out
 */
NAN_METHOD(GitTreeBuilder::Insert) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError("String filename is required.");
  }
  if (args.Length() == 1 || !args[1]->IsObject()) {
    return NanThrowError("Oid id is required.");
  }
  if (args.Length() == 2 || !args[2]->IsNumber()) {
    return NanThrowError("Number filemode is required.");
  }

  const git_tree_entry * out = 0;
  const char * from_filename;
            String::Utf8Value filename(args[0]->ToString());
      from_filename = strdup(*filename);
        const git_oid * from_id;
            from_id = ObjectWrap::Unwrap<GitOid>(args[1]->ToObject())->GetValue();
        git_filemode_t from_filemode;
            from_filemode = (git_filemode_t) (int)  args[2]->ToNumber()->Value();
      
  int result = git_treebuilder_insert(
    &out
    , ObjectWrap::Unwrap<GitTreeBuilder>(args.This())->GetValue()
    , from_filename
    , from_id
    , from_filemode
  );
  free((void *)from_filename);
  if (result != GIT_OK) {
    if (giterr_last()) {
      return NanThrowError(giterr_last()->message);
    } else {
      return NanThrowError("Unknown Error");
    }
  }

  Handle<Value> to;
    if (out != NULL) {
    out = (const git_tree_entry * )git_tree_entry_dup(out);
  }
  if (out != NULL) {
    to = GitTreeEntry::New((void *)out);
  } else {
    to = NanNull();
  }
  NanReturnValue(to);
}

/**
 * @param {String} filename
 */
NAN_METHOD(GitTreeBuilder::GitTreebuilderRemove) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError("String filename is required.");
  }

  const char * from_filename;
            String::Utf8Value filename(args[0]->ToString());
      from_filename = strdup(*filename);
      
  int result = git_treebuilder_remove(
    ObjectWrap::Unwrap<GitTreeBuilder>(args.This())->GetValue()
    , from_filename
  );
  free((void *)from_filename);
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
 * @param {Repository} repo
 * @param {Oid} callback
 */
NAN_METHOD(GitTreeBuilder::Write) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsObject()) {
    return NanThrowError("Repository repo is required.");
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  WriteBaton* baton = new WriteBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->id = (git_oid *)malloc(sizeof(git_oid ));
    git_repository * from_repo;
            from_repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
          baton->repo = from_repo;
    baton->bld = ObjectWrap::Unwrap<GitTreeBuilder>(args.This())->GetValue();

  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[1]));
  WriteWorker *worker = new WriteWorker(baton, callback);
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("repo", args[0]->ToObject());
  worker->SaveToPersistent("bld", args.This());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitTreeBuilder::WriteWorker::Execute() {
  int result = git_treebuilder_write(
    baton->id, 
    baton->repo, 
    baton->bld
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitTreeBuilder::WriteWorker::HandleOKCallback() {
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
  delete baton;
}

Persistent<Function> GitTreeBuilder::constructor_template;

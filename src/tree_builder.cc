/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
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

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanSymbol("TreeBuilder"));
  
  NODE_SET_METHOD(tpl, "create", Create);
  NODE_SET_PROTOTYPE_METHOD(tpl, "clear", Clear);
  NODE_SET_METHOD(tpl, "size", Size);
  NODE_SET_PROTOTYPE_METHOD(tpl, "get", Get);
  NODE_SET_PROTOTYPE_METHOD(tpl, "insert", Insert);
  NODE_SET_PROTOTYPE_METHOD(tpl, "gitTreebuilderRemove", GitTreebuilderRemove);
  NODE_SET_PROTOTYPE_METHOD(tpl, "write", Write);

  NanAssignPersistent(FunctionTemplate, constructor_template, tpl);
  target->Set(String::NewSymbol("TreeBuilder"), tpl->GetFunction());
}

NAN_METHOD(GitTreeBuilder::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError(String::New("git_treebuilder is required."));
  }

  GitTreeBuilder* object = new GitTreeBuilder((git_treebuilder *) External::Cast(*args[0])->Value());
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitTreeBuilder::New(void *raw) {
  NanScope();
  Handle<Value> argv[1] = { External::New((void *)raw) };
  Local<Object> instance;
  Local<FunctionTemplate> constructorHandle = NanPersistentToLocal(constructor_template);
  instance = constructorHandle->GetFunction()->NewInstance(1, argv);
  return scope.Close(instance);
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
      return NanThrowError(String::New(giterr_last()->message));
    } else {
      return NanThrowError(String::New("Unkown Error"));
    }
  }

  Handle<Value> to;
    if (out != NULL) {
    to = GitTreeBuilder::New((void *)out);
  } else {
    to = Null();
  }
  NanReturnValue(to);
}

/**
 * @param {TreeBuilder} bld
 */
NAN_METHOD(GitTreeBuilder::Clear) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return NanThrowError(String::New("TreeBuilder bld is required."));
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
    to = Uint32::New(result);
  NanReturnValue(to);
}

/**
 * @param {String} filename
 * @return {TreeEntry} result
 */
NAN_METHOD(GitTreeBuilder::Get) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError(String::New("String filename is required."));
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
    to = Null();
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
    return NanThrowError(String::New("String filename is required."));
  }
  if (args.Length() == 1 || !args[1]->IsObject()) {
    return NanThrowError(String::New("Oid id is required."));
  }
  if (args.Length() == 2 || !args[2]->IsNumber()) {
    return NanThrowError(String::New("Number filemode is required."));
  }

  const git_tree_entry * out = 0;
  const char * from_filename;
            String::Utf8Value filename(args[0]->ToString());
      from_filename = strdup(*filename);
        const git_oid * from_id;
            from_id = ObjectWrap::Unwrap<GitOid>(args[1]->ToObject())->GetValue();
        git_filemode_t from_filemode;
            from_filemode = (git_filemode_t) args[2]->ToNumber()->Value();
      
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
      return NanThrowError(String::New(giterr_last()->message));
    } else {
      return NanThrowError(String::New("Unkown Error"));
    }
  }

  Handle<Value> to;
    if (out != NULL) {
    out = (const git_tree_entry * )git_tree_entry_dup(out);
  }
  if (out != NULL) {
    to = GitTreeEntry::New((void *)out);
  } else {
    to = Null();
  }
  NanReturnValue(to);
}

/**
 * @param {String} filename
 */
NAN_METHOD(GitTreeBuilder::GitTreebuilderRemove) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError(String::New("String filename is required."));
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
      return NanThrowError(String::New(giterr_last()->message));
    } else {
      return NanThrowError(String::New("Unkown Error"));
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
    return NanThrowError(String::New("Repository repo is required."));
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError(String::New("Callback is required and must be a Function."));
  }

  WriteBaton* baton = new WriteBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->id = (git_oid *)malloc(sizeof(git_oid ));
  NanAssignPersistent(Value, baton->repoReference, args[0]);
  git_repository * from_repo;
  from_repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
  baton->repo = from_repo;
  NanAssignPersistent(Value, baton->bldReference, args.This());
  baton->bld = ObjectWrap::Unwrap<GitTreeBuilder>(args.This())->GetValue();
  NanAssignPersistent(Function, baton->callback, Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, WriteWork, (uv_after_work_cb)WriteAfterWork);

  NanReturnUndefined();
}

void GitTreeBuilder::WriteWork(uv_work_t *req) {
  WriteBaton *baton = static_cast<WriteBaton *>(req->data);
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

void GitTreeBuilder::WriteAfterWork(uv_work_t *req) {
  NanScope();
  WriteBaton *baton = static_cast<WriteBaton *>(req->data);

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
        free(baton->id);
      }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  baton->repoReference.Dispose();
  baton->bldReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

Persistent<FunctionTemplate> GitTreeBuilder::constructor_template;

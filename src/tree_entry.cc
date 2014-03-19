/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/functions/copy.h"

#include "../include/tree_entry.h"
#include "../include/oid.h"
#include "../include/repo.h"
#include "../include/object.h"

using namespace v8;
using namespace node;

GitTreeEntry::GitTreeEntry(git_tree_entry *raw) {
  this->raw = raw;
}

GitTreeEntry::~GitTreeEntry() {
  git_tree_entry_free(this->raw);
}

void GitTreeEntry::Initialize(Handle<v8::Object> target) {
  NanScope();

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanSymbol("TreeEntry"));
  
  NODE_SET_PROTOTYPE_METHOD(tpl, "name", Name);
  NODE_SET_PROTOTYPE_METHOD(tpl, "oid", Oid);
  NODE_SET_PROTOTYPE_METHOD(tpl, "type", Type);
  NODE_SET_PROTOTYPE_METHOD(tpl, "filemode", filemode);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getObject", GetObject);

  NanAssignPersistent(FunctionTemplate, constructor_template, tpl);
  target->Set(String::NewSymbol("TreeEntry"), tpl->GetFunction());
}

NAN_METHOD(GitTreeEntry::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError(String::New("git_tree_entry is required."));
  }

  GitTreeEntry* object = new GitTreeEntry((git_tree_entry *) External::Cast(*args[0])->Value());
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitTreeEntry::New(void *raw) {
  NanScope();
  Handle<Value> argv[1] = { External::New((void *)raw) };
  Local<Object> instance;
  Local<FunctionTemplate> constructorHandle = NanPersistentToLocal(constructor_template);
  instance = constructorHandle->GetFunction()->NewInstance(1, argv);
  return scope.Close(instance);
}

git_tree_entry *GitTreeEntry::GetValue() {
  return this->raw;
}


/**
 * @return {String} result
 */
NAN_METHOD(GitTreeEntry::Name) {
  NanScope();
  

  const char * result = git_tree_entry_name(
    ObjectWrap::Unwrap<GitTreeEntry>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = String::New(result);
  NanReturnValue(to);
}

/**
 * @return {Oid} result
 */
NAN_METHOD(GitTreeEntry::Oid) {
  NanScope();
  

  const git_oid * result = git_tree_entry_id(
    ObjectWrap::Unwrap<GitTreeEntry>(args.This())->GetValue()
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
 * @return {Number} result
 */
NAN_METHOD(GitTreeEntry::Type) {
  NanScope();
  

  git_otype result = git_tree_entry_type(
    ObjectWrap::Unwrap<GitTreeEntry>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = Number::New(result);
  NanReturnValue(to);
}

/**
 * @return {Number} result
 */
NAN_METHOD(GitTreeEntry::filemode) {
  NanScope();
  

  git_filemode_t result = git_tree_entry_filemode(
    ObjectWrap::Unwrap<GitTreeEntry>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = Number::New(result);
  NanReturnValue(to);
}

#include "../include/functions/copy.h"

/**
 * @param {Repository} repo
 * @param {Object} callback
 */
NAN_METHOD(GitTreeEntry::GetObject) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsObject()) {
    return NanThrowError(String::New("Repository repo is required."));
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError(String::New("Callback is required and must be a Function."));
  }

  GetObjectBaton* baton = new GetObjectBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  NanAssignPersistent(Value, baton->repoReference, args[0]);
  git_repository * from_repo;
  from_repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
  baton->repo = from_repo;
  NanAssignPersistent(Value, baton->entryReference, args.This());
  baton->entry = ObjectWrap::Unwrap<GitTreeEntry>(args.This())->GetValue();
  NanAssignPersistent(Function, baton->callback, Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, GetObjectWork, (uv_after_work_cb)GetObjectAfterWork);

  NanReturnUndefined();
}

void GitTreeEntry::GetObjectWork(uv_work_t *req) {
  GetObjectBaton *baton = static_cast<GetObjectBaton *>(req->data);
  int result = git_tree_entry_to_object(
    &baton->object_out, 
    baton->repo, 
    baton->entry
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitTreeEntry::GetObjectAfterWork(uv_work_t *req) {
  NanScope();
  GetObjectBaton *baton = static_cast<GetObjectBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->object_out != NULL) {
    to = GitObject::New((void *)baton->object_out);
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
  baton->entryReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

Persistent<FunctionTemplate> GitTreeEntry::constructor_template;

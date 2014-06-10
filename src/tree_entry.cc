/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <nan.h>
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

  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanNew<String>("TreeEntry"));

  NODE_SET_PROTOTYPE_METHOD(tpl, "name", Name);
  NODE_SET_PROTOTYPE_METHOD(tpl, "oid", Oid);
  NODE_SET_PROTOTYPE_METHOD(tpl, "type", Type);
  NODE_SET_PROTOTYPE_METHOD(tpl, "filemode", filemode);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getObject", GetObject);


  Local<Function> _constructor_template = tpl->GetFunction();
  NanAssignPersistent(constructor_template, _constructor_template);
  target->Set(NanNew<String>("TreeEntry"), _constructor_template);
}

NAN_METHOD(GitTreeEntry::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError("git_tree_entry is required.");
  }
  GitTreeEntry* object = new GitTreeEntry(static_cast<git_tree_entry *>(Handle<External>::Cast(args[0])->Value()));
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitTreeEntry::New(void *raw) {
  NanEscapableScope();
  Handle<Value> argv[1] = { NanNew<External>((void *)raw) };
  return NanEscapeScope(NanNew<Function>(GitTreeEntry::constructor_template)->NewInstance(1, argv));
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
    to = NanNew<String>(result);
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
    to = NanNull();
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
    to = NanNew<Number>(result);
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
    to = NanNew<Number>(result);
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
    return NanThrowError("Repository repo is required.");
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  GetObjectBaton* baton = new GetObjectBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
    git_repository * from_repo;
            from_repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
          baton->repo = from_repo;
    baton->entry = ObjectWrap::Unwrap<GitTreeEntry>(args.This())->GetValue();

  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[1]));
  GetObjectWorker *worker = new GetObjectWorker(baton, callback);
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("repo", args[0]->ToObject());
  worker->SaveToPersistent("entry", args.This());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitTreeEntry::GetObjectWorker::Execute() {
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

void GitTreeEntry::GetObjectWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->object_out != NULL) {
    to = GitObject::New((void *)baton->object_out);
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

Persistent<Function> GitTreeEntry::constructor_template;

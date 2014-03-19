/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/functions/copy.h"

#include "../include/commit.h"
#include "../include/oid.h"
#include "../include/repo.h"
#include "../include/signature.h"
#include "../include/tree.h"

using namespace v8;
using namespace node;

GitCommit::GitCommit(git_commit *raw) {
  this->raw = raw;
}

GitCommit::~GitCommit() {
  git_commit_free(this->raw);
}

void GitCommit::Initialize(Handle<v8::Object> target) {
  NanScope();

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanSymbol("Commit"));

  NODE_SET_PROTOTYPE_METHOD(tpl, "oid", Oid);
  NODE_SET_PROTOTYPE_METHOD(tpl, "messageEncoding", MessageEncoding);
  NODE_SET_PROTOTYPE_METHOD(tpl, "message", Message);
  NODE_SET_PROTOTYPE_METHOD(tpl, "time", Time);
  NODE_SET_PROTOTYPE_METHOD(tpl, "offset", Offset);
  NODE_SET_PROTOTYPE_METHOD(tpl, "committer", Committer);
  NODE_SET_PROTOTYPE_METHOD(tpl, "author", Author);
  NODE_SET_PROTOTYPE_METHOD(tpl, "treeId", TreeId);
  NODE_SET_PROTOTYPE_METHOD(tpl, "parentCount", ParentCount);
  NODE_SET_PROTOTYPE_METHOD(tpl, "parentId", ParentId);
  NODE_SET_PROTOTYPE_METHOD(tpl, "nthGenAncestor", NthGenAncestor);

  NanAssignPersistent(FunctionTemplate, constructor_template, tpl);
  target->Set(String::NewSymbol("Commit"), tpl->GetFunction());
}

NAN_METHOD(GitCommit::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError(String::New("git_commit is required."));
  }

  GitCommit* object = new GitCommit((git_commit *) External::Cast(*args[0])->Value());
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitCommit::New(void *raw) {
  NanScope();
  Handle<Value> argv[1] = { External::New((void *)raw) };
  Local<Object> instance;
  Local<FunctionTemplate> constructorHandle = NanPersistentToLocal(constructor_template);
  instance = constructorHandle->GetFunction()->NewInstance(1, argv);
  return scope.Close(instance);
}

git_commit *GitCommit::GetValue() {
  return this->raw;
}


/**
 * @return {Oid} result
 */
NAN_METHOD(GitCommit::Oid) {
  NanScope();
  

  const git_oid * result = git_commit_id(
    ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
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
 * @return {String} result
 */
NAN_METHOD(GitCommit::MessageEncoding) {
  NanScope();
  

  const char * result = git_commit_message_encoding(
    ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = String::New(result);
  NanReturnValue(to);
}

/**
 * @return {String} result
 */
NAN_METHOD(GitCommit::Message) {
  NanScope();
  

  const char * result = git_commit_message(
    ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = String::New(result);
  NanReturnValue(to);
}

/**
 * @return {Number} result
 */
NAN_METHOD(GitCommit::Time) {
  NanScope();
  

  git_time_t result = git_commit_time(
    ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = Number::New(result);
  NanReturnValue(to);
}

/**
 * @return {Number} result
 */
NAN_METHOD(GitCommit::Offset) {
  NanScope();  

  int result = git_commit_time_offset(
    ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = Integer::New(result);
  NanReturnValue(to);
}

/**
 * @return {Signature} result
 */
NAN_METHOD(GitCommit::Committer) {
  NanScope();
  

  const git_signature * result = git_commit_committer(
    ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
  );

  Handle<Value> to;
    if (result != NULL) {
    result = (const git_signature * )git_signature_dup(result);
  }
  if (result != NULL) {
    to = GitSignature::New((void *)result);
  } else {
    to = Null();
  }
  NanReturnValue(to);
}

/**
 * @return {Signature} result
 */
NAN_METHOD(GitCommit::Author) {
  NanScope();
  

  const git_signature * result = git_commit_author(
    ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
  );

  Handle<Value> to;
    if (result != NULL) {
    result = (const git_signature * )git_signature_dup(result);
  }
  if (result != NULL) {
    to = GitSignature::New((void *)result);
  } else {
    to = Null();
  }
  NanReturnValue(to);
}

/**
 * @return {Oid} result
 */
NAN_METHOD(GitCommit::TreeId) {
  NanScope();
  

  const git_oid * result = git_commit_tree_id(
    ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
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
NAN_METHOD(GitCommit::ParentCount) {
  NanScope();
  

  unsigned int result = git_commit_parentcount(
    ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = Uint32::New(result);
  NanReturnValue(to);
}

/**
 * @param {Number} n
 * @return {Oid} result
 */
NAN_METHOD(GitCommit::ParentId) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsUint32()) {
    return NanThrowError(String::New("Number n is required."));
  }

  unsigned int from_n;
            from_n = (unsigned int) args[0]->ToUint32()->Value();
      
  const git_oid * result = git_commit_parent_id(
    ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
    , from_n
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
 * @param {Number} n
 * @return {Commit} ancestor
 */
NAN_METHOD(GitCommit::NthGenAncestor) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsUint32()) {
    return NanThrowError(String::New("Number n is required."));
  }

  git_commit * ancestor = 0;
  unsigned int from_n;
            from_n = (unsigned int) args[0]->ToUint32()->Value();
      
  int result = git_commit_nth_gen_ancestor(
    &ancestor
    , ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
    , from_n
  );
  if (result != GIT_OK) {
    if (giterr_last()) {
      return NanThrowError(String::New(giterr_last()->message));
    } else {
      return NanThrowError(String::New("Unkown Error"));
    }
  }

  Handle<Value> to;
    if (ancestor != NULL) {
    to = GitCommit::New(ancestor);
  } else {
    to = Null();
  }
  NanReturnValue(to);
}

Persistent<FunctionTemplate> GitCommit::constructor_template;

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
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Commit"));

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


  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Commit"), constructor_template);
}

Handle<Value> GitCommit::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return ThrowException(Exception::Error(String::New("git_commit is required.")));
  }

  GitCommit* object = new GitCommit((git_commit *) External::Unwrap(args[0]));
  object->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> GitCommit::New(void *raw) {
  HandleScope scope;
  Handle<Value> argv[1] = { External::New((void *)raw) };
  return scope.Close(GitCommit::constructor_template->NewInstance(1, argv));
}

git_commit *GitCommit::GetValue() {
  return this->raw;
}


/**
 * @return {Oid} result
 */
Handle<Value> GitCommit::Oid(const Arguments& args) {
  HandleScope scope;
  

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
  return scope.Close(to);
}

/**
 * @return {String} result
 */
Handle<Value> GitCommit::MessageEncoding(const Arguments& args) {
  HandleScope scope;
  

  const char * result = git_commit_message_encoding(
    ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = String::New(result);
  return scope.Close(to);
}

/**
 * @return {String} result
 */
Handle<Value> GitCommit::Message(const Arguments& args) {
  HandleScope scope;
  

  const char * result = git_commit_message(
    ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = String::New(result);
  return scope.Close(to);
}

/**
 * @return {Number} result
 */
Handle<Value> GitCommit::Time(const Arguments& args) {
  HandleScope scope;
  

  git_time_t result = git_commit_time(
    ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = Number::New(result);
  return scope.Close(to);
}

/**
 * @return {Number} result
 */
Handle<Value> GitCommit::Offset(const Arguments& args) {
  HandleScope scope;
  

  int result = git_commit_time_offset(
    ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = Integer::New(result);
  return scope.Close(to);
}

/**
 * @return {Signature} result
 */
Handle<Value> GitCommit::Committer(const Arguments& args) {
  HandleScope scope;
  

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
  return scope.Close(to);
}

/**
 * @return {Signature} result
 */
Handle<Value> GitCommit::Author(const Arguments& args) {
  HandleScope scope;
  

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
  return scope.Close(to);
}

/**
 * @return {Oid} result
 */
Handle<Value> GitCommit::TreeId(const Arguments& args) {
  HandleScope scope;
  

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
  return scope.Close(to);
}

/**
 * @return {Number} result
 */
Handle<Value> GitCommit::ParentCount(const Arguments& args) {
  HandleScope scope;
  

  unsigned int result = git_commit_parentcount(
    ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = Uint32::New(result);
  return scope.Close(to);
}

/**
 * @param {Number} n
 * @return {Oid} result
 */
Handle<Value> GitCommit::ParentId(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsUint32()) {
    return ThrowException(Exception::Error(String::New("Number n is required.")));
  }

  unsigned int from_n;
            from_n = (unsigned int)   args[0]->ToUint32()->Value();
      
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
  return scope.Close(to);
}

/**
 * @param {Number} n
 * @return {Commit} ancestor
 */
Handle<Value> GitCommit::NthGenAncestor(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsUint32()) {
    return ThrowException(Exception::Error(String::New("Number n is required.")));
  }

  git_commit * ancestor = 0;
  unsigned int from_n;
            from_n = (unsigned int)   args[0]->ToUint32()->Value();
      
  int result = git_commit_nth_gen_ancestor(
    &ancestor
    , ObjectWrap::Unwrap<GitCommit>(args.This())->GetValue()
    , from_n
  );
  if (result != GIT_OK) {
    if (giterr_last()) {
      return ThrowException(Exception::Error(String::New(giterr_last()->message)));
    } else {
      return ThrowException(Exception::Error(String::New("Unkown Error")));
    }
  }

  Handle<Value> to;
    if (ancestor != NULL) {
    to = GitCommit::New((void *)ancestor);
  } else {
    to = Null();
  }
  return scope.Close(to);
}

Persistent<Function> GitCommit::constructor_template;

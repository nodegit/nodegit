/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/functions/copy.h"

#include "../include/diff_list.h"
#include "../include/diff_options.h"
#include "../include/diff_find_options.h"
#include "../include/repo.h"
#include "../include/tree.h"
#include "../include/index.h"
#include "../include/patch.h"
#include "../include/delta.h"

using namespace v8;
using namespace node;

GitDiffList::GitDiffList(git_diff_list *raw) {
  this->raw = raw;
}

GitDiffList::~GitDiffList() {
  git_diff_list_free(this->raw);
}

void GitDiffList::Initialize(Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("DiffList"));

  NODE_SET_PROTOTYPE_METHOD(tpl, "merge", Merge);
  NODE_SET_PROTOTYPE_METHOD(tpl, "findSimilar", FindSimilar);
  NODE_SET_PROTOTYPE_METHOD(tpl, "size", Size);
  NODE_SET_METHOD(tpl, "numDeltasOfType", NumDeltasOfType);
  NODE_SET_PROTOTYPE_METHOD(tpl, "patch", Patch);


  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("DiffList"), constructor_template);
}

Handle<Value> GitDiffList::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return ThrowException(Exception::Error(String::New("git_diff_list is required.")));
  }

  GitDiffList* object = new GitDiffList((git_diff_list *) External::Unwrap(args[0]));
  object->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> GitDiffList::New(void *raw) {
  HandleScope scope;
  Handle<Value> argv[1] = { External::New((void *)raw) };
  return scope.Close(GitDiffList::constructor_template->NewInstance(1, argv));
}

git_diff_list *GitDiffList::GetValue() {
  return this->raw;
}


/**
 * @param {DiffList} from
 */
Handle<Value> GitDiffList::Merge(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("DiffList from is required.")));
  }

  const git_diff_list * from_from;
            from_from = ObjectWrap::Unwrap<GitDiffList>(args[0]->ToObject())->GetValue();
      
  int result = git_diff_merge(
    ObjectWrap::Unwrap<GitDiffList>(args.This())->GetValue()
    , from_from
  );
  if (result != GIT_OK) {
    if (giterr_last()) {
      return ThrowException(Exception::Error(String::New(giterr_last()->message)));
    } else {
      return ThrowException(Exception::Error(String::New("Unkown Error")));
    }
  }

  return Undefined();
}

/**
 * @param {DiffFindOptions} options
 */
Handle<Value> GitDiffList::FindSimilar(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("DiffFindOptions options is required.")));
  }

  git_diff_find_options * from_options;
            from_options = ObjectWrap::Unwrap<GitDiffFindOptions>(args[0]->ToObject())->GetValue();
      
  int result = git_diff_find_similar(
    ObjectWrap::Unwrap<GitDiffList>(args.This())->GetValue()
    , from_options
  );
  if (result != GIT_OK) {
    if (giterr_last()) {
      return ThrowException(Exception::Error(String::New(giterr_last()->message)));
    } else {
      return ThrowException(Exception::Error(String::New("Unkown Error")));
    }
  }

  return Undefined();
}

/**
 * @return {Number} result
 */
Handle<Value> GitDiffList::Size(const Arguments& args) {
  HandleScope scope;
  

  size_t result = git_diff_num_deltas(
    ObjectWrap::Unwrap<GitDiffList>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = Uint32::New(result);
  return scope.Close(to);
}

/**
 * @param {DiffList} diff
 * @param {Number} type
 * @return {Number} result
 */
Handle<Value> GitDiffList::NumDeltasOfType(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("DiffList diff is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number type is required.")));
  }

  git_diff_list * from_diff;
            from_diff = ObjectWrap::Unwrap<GitDiffList>(args[0]->ToObject())->GetValue();
        git_delta_t from_type;
            from_type = (git_delta_t)   args[1]->ToInt32()->Value();
      
  size_t result = git_diff_num_deltas_of_type(
    from_diff
    , from_type
  );

  Handle<Value> to;
    to = Uint32::New(result);
  return scope.Close(to);
}

/**
 * @param {Number} idx
 * @return {Patch} patch_out
 * @return {Delta} delta_out
 */
Handle<Value> GitDiffList::Patch(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsUint32()) {
    return ThrowException(Exception::Error(String::New("Number idx is required.")));
  }

  git_diff_patch * patch_out = 0;
  const git_diff_delta * delta_out = 0;
  size_t from_idx;
            from_idx = (size_t)   args[0]->ToUint32()->Value();
      
  int result = git_diff_get_patch(
    &patch_out
    , &delta_out
    , ObjectWrap::Unwrap<GitDiffList>(args.This())->GetValue()
    , from_idx
  );
  if (result != GIT_OK) {
    if (giterr_last()) {
      return ThrowException(Exception::Error(String::New(giterr_last()->message)));
    } else {
      return ThrowException(Exception::Error(String::New("Unkown Error")));
    }
  }

  Handle<Object> toReturn = Object::New();
  Handle<Value> to;
      if (patch_out != NULL) {
    to = GitPatch::New((void *)patch_out);
  } else {
    to = Null();
  }
    toReturn->Set(String::NewSymbol("patch"), to);

      if (delta_out != NULL) {
    delta_out = (const git_diff_delta * )git_diff_delta_dup(delta_out);
  }
  if (delta_out != NULL) {
    to = GitDelta::New((void *)delta_out);
  } else {
    to = Null();
  }
    toReturn->Set(String::NewSymbol("delta"), to);

  return scope.Close(toReturn);
}

Persistent<Function> GitDiffList::constructor_template;

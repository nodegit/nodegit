/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <nan.h>
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
  NanScope();

  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanNew<String>("DiffList"));

  NODE_SET_PROTOTYPE_METHOD(tpl, "merge", Merge);
  NODE_SET_PROTOTYPE_METHOD(tpl, "findSimilar", FindSimilar);
  NODE_SET_PROTOTYPE_METHOD(tpl, "size", Size);
  NODE_SET_METHOD(tpl, "numDeltasOfType", NumDeltasOfType);
  NODE_SET_PROTOTYPE_METHOD(tpl, "patch", Patch);


  Local<Function> _constructor_template = tpl->GetFunction();
  NanAssignPersistent(constructor_template, _constructor_template);
  target->Set(NanNew<String>("DiffList"), _constructor_template);
}

NAN_METHOD(GitDiffList::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError("git_diff_list is required.");
  }
  GitDiffList* object = new GitDiffList(static_cast<git_diff_list *>(Handle<External>::Cast(args[0])->Value()));
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitDiffList::New(void *raw) {
  NanEscapableScope();
  Handle<Value> argv[1] = { NanNew<External>((void *)raw) };
  return NanEscapeScope(NanNew<Function>(GitDiffList::constructor_template)->NewInstance(1, argv));
}

git_diff_list *GitDiffList::GetValue() {
  return this->raw;
}


/**
 * @param {DiffList} from
 */
NAN_METHOD(GitDiffList::Merge) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return NanThrowError("DiffList from is required.");
  }

  const git_diff_list * from_from;
            from_from = ObjectWrap::Unwrap<GitDiffList>(args[0]->ToObject())->GetValue();
      
  int result = git_diff_merge(
    ObjectWrap::Unwrap<GitDiffList>(args.This())->GetValue()
    , from_from
  );
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
 * @param {DiffFindOptions} options
 */
NAN_METHOD(GitDiffList::FindSimilar) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return NanThrowError("DiffFindOptions options is required.");
  }

  git_diff_find_options * from_options;
            from_options = ObjectWrap::Unwrap<GitDiffFindOptions>(args[0]->ToObject())->GetValue();
      
  int result = git_diff_find_similar(
    ObjectWrap::Unwrap<GitDiffList>(args.This())->GetValue()
    , from_options
  );
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
 * @return {Number} result
 */
NAN_METHOD(GitDiffList::Size) {
  NanScope();
  

  size_t result = git_diff_num_deltas(
    ObjectWrap::Unwrap<GitDiffList>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = NanNew<Uint32>((uint32_t)result);
  NanReturnValue(to);
}

/**
 * @param {DiffList} diff
 * @param {Number} type
 * @return {Number} result
 */
NAN_METHOD(GitDiffList::NumDeltasOfType) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return NanThrowError("DiffList diff is required.");
  }
  if (args.Length() == 1 || !args[1]->IsInt32()) {
    return NanThrowError("Number type is required.");
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
    to = NanNew<Uint32>((uint32_t)result);
  NanReturnValue(to);
}

/**
 * @param {Number} idx
 * @return {Patch} patch_out
 * @return {Delta} delta_out
 */
NAN_METHOD(GitDiffList::Patch) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsUint32()) {
    return NanThrowError("Number idx is required.");
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
      return NanThrowError(giterr_last()->message);
    } else {
      return NanThrowError("Unknown Error");
    }
  }

  Handle<Object> toReturn = NanNew<Object>();
  Handle<Value> to;
      if (patch_out != NULL) {
    to = GitPatch::New((void *)patch_out);
  } else {
    to = NanNull();
  }
    toReturn->Set(NanNew<String>("patch"), to);

      if (delta_out != NULL) {
    delta_out = (const git_diff_delta * )git_diff_delta_dup(delta_out);
  }
  if (delta_out != NULL) {
    to = GitDelta::New((void *)delta_out);
  } else {
    to = NanNull();
  }
    toReturn->Set(NanNew<String>("delta"), to);

  NanReturnValue(toReturn);
}

Persistent<Function> GitDiffList::constructor_template;

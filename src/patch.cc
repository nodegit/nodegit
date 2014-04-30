/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/functions/copy.h"

#include "../include/patch.h"
#include "../include/delta.h"
#include "../include/diff_range.h"

using namespace v8;
using namespace node;

GitPatch::GitPatch(git_diff_patch *raw) {
  this->raw = raw;
}

GitPatch::~GitPatch() {
  git_diff_patch_free(this->raw);
}

void GitPatch::Initialize(Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Patch"));

  NODE_SET_PROTOTYPE_METHOD(tpl, "delta", Delta);
  NODE_SET_PROTOTYPE_METHOD(tpl, "size", Size);
  NODE_SET_PROTOTYPE_METHOD(tpl, "stats", Stats);
  NODE_SET_PROTOTYPE_METHOD(tpl, "hunk", Hunk);
  NODE_SET_PROTOTYPE_METHOD(tpl, "lines", Lines);
  NODE_SET_PROTOTYPE_METHOD(tpl, "line", Line);
  NODE_SET_METHOD(tpl, "toString", ToString);


  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Patch"), constructor_template);
}

Handle<Value> GitPatch::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return ThrowException(Exception::Error(String::New("git_diff_patch is required.")));
  }

  GitPatch* object = new GitPatch((git_diff_patch *) External::Unwrap(args[0]));
  object->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> GitPatch::New(void *raw) {
  HandleScope scope;
  Handle<Value> argv[1] = { External::New((void *)raw) };
  return scope.Close(GitPatch::constructor_template->NewInstance(1, argv));
}

git_diff_patch *GitPatch::GetValue() {
  return this->raw;
}


/**
 * @return {Delta} result
 */
Handle<Value> GitPatch::Delta(const Arguments& args) {
  HandleScope scope;
  

  const git_diff_delta * result = git_diff_patch_delta(
    ObjectWrap::Unwrap<GitPatch>(args.This())->GetValue()
  );

  Handle<Value> to;
    if (result != NULL) {
    result = (const git_diff_delta * )git_diff_delta_dup(result);
  }
  if (result != NULL) {
    to = GitDelta::New((void *)result);
  } else {
    to = Null();
  }
  return scope.Close(to);
}

/**
 * @return {Number} result
 */
Handle<Value> GitPatch::Size(const Arguments& args) {
  HandleScope scope;
  

  size_t result = git_diff_patch_num_hunks(
    ObjectWrap::Unwrap<GitPatch>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = Uint32::New(result);
  return scope.Close(to);
}

/**
 * @return {Number} total_context
 * @return {Number} total_additions
 * @return {Number} total_deletions
 */
Handle<Value> GitPatch::Stats(const Arguments& args) {
  HandleScope scope;
  
  size_t total_context = 0;
  size_t total_additions = 0;
  size_t total_deletions = 0;

  int result = git_diff_patch_line_stats(
    &total_context
    , &total_additions
    , &total_deletions
    , ObjectWrap::Unwrap<GitPatch>(args.This())->GetValue()
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
      to = Integer::New(total_context);
    toReturn->Set(String::NewSymbol("total_context"), to);

      to = Integer::New(total_additions);
    toReturn->Set(String::NewSymbol("total_additions"), to);

      to = Integer::New(total_deletions);
    toReturn->Set(String::NewSymbol("total_deletions"), to);

  return scope.Close(toReturn);
}

/**
 * @param {Number} hunk_idx
 * @return {DiffRange} range
 * @return {String} header
 * @return {Number} header_len
 * @return {Number} lines_in_hunk
 */
Handle<Value> GitPatch::Hunk(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsUint32()) {
    return ThrowException(Exception::Error(String::New("Number hunk_idx is required.")));
  }

  const git_diff_range * range = 0;
  const char * header = 0;
  size_t header_len = 0;
  size_t lines_in_hunk = 0;
  size_t from_hunk_idx;
            from_hunk_idx = (size_t)   args[0]->ToUint32()->Value();
      
  int result = git_diff_patch_get_hunk(
    &range
    , &header
    , &header_len
    , &lines_in_hunk
    , ObjectWrap::Unwrap<GitPatch>(args.This())->GetValue()
    , from_hunk_idx
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
      if (range != NULL) {
    range = (const git_diff_range * )git_diff_range_dup(range);
  }
  if (range != NULL) {
    to = GitDiffRange::New((void *)range);
  } else {
    to = Null();
  }
    toReturn->Set(String::NewSymbol("range"), to);

      to = String::New(header);
    toReturn->Set(String::NewSymbol("header"), to);

      to = Uint32::New(header_len);
    toReturn->Set(String::NewSymbol("headerLength"), to);

      to = Uint32::New(lines_in_hunk);
    toReturn->Set(String::NewSymbol("lines"), to);

  return scope.Close(toReturn);
}

/**
 * @param {Number} hunk_idx
 * @return {Number} result
 */
Handle<Value> GitPatch::Lines(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsUint32()) {
    return ThrowException(Exception::Error(String::New("Number hunk_idx is required.")));
  }

  size_t from_hunk_idx;
            from_hunk_idx = (size_t)   args[0]->ToUint32()->Value();
      
  int result = git_diff_patch_num_lines_in_hunk(
    ObjectWrap::Unwrap<GitPatch>(args.This())->GetValue()
    , from_hunk_idx
  );

  Handle<Value> to;
    to = Int32::New(result);
  return scope.Close(to);
}

/**
 * @param {Number} hunk_idx
 * @param {Number} line_of_hunk
 * @return {Number} line_origin
 * @return {String} content
 * @return {Number} content_len
 * @return {Number} old_lineno
 * @return {Number} new_lineno
 */
Handle<Value> GitPatch::Line(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsUint32()) {
    return ThrowException(Exception::Error(String::New("Number hunk_idx is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsUint32()) {
    return ThrowException(Exception::Error(String::New("Number line_of_hunk is required.")));
  }

  char line_origin = 0;
  const char * content = 0;
  size_t content_len = 0;
  int old_lineno = 0;
  int new_lineno = 0;
  size_t from_hunk_idx;
            from_hunk_idx = (size_t)   args[0]->ToUint32()->Value();
        size_t from_line_of_hunk;
            from_line_of_hunk = (size_t)   args[1]->ToUint32()->Value();
      
  int result = git_diff_patch_get_line_in_hunk(
    &line_origin
    , &content
    , &content_len
    , &old_lineno
    , &new_lineno
    , ObjectWrap::Unwrap<GitPatch>(args.This())->GetValue()
    , from_hunk_idx
    , from_line_of_hunk
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
      to = Integer::New(line_origin);
    toReturn->Set(String::NewSymbol("lineOrigin"), to);

      to = String::New(content, content_len);
    toReturn->Set(String::NewSymbol("content"), to);

      to = Uint32::New(content_len);
    toReturn->Set(String::NewSymbol("length"), to);

      to = Int32::New(old_lineno);
    toReturn->Set(String::NewSymbol("oldLineNumber"), to);

      to = Int32::New(new_lineno);
    toReturn->Set(String::NewSymbol("newLineNumber"), to);

  return scope.Close(toReturn);
}

/**
 * @return {String} string
 */
Handle<Value> GitPatch::ToString(const Arguments& args) {
  HandleScope scope;
  
  char * string = 0;

  int result = git_diff_patch_to_str(
    &string
    , ObjectWrap::Unwrap<GitPatch>(args.This())->GetValue()
  );
  if (result != GIT_OK) {
    if (giterr_last()) {
      return ThrowException(Exception::Error(String::New(giterr_last()->message)));
    } else {
      return ThrowException(Exception::Error(String::New("Unkown Error")));
    }
  }

  Handle<Value> to;
    to = String::New(string);
  free(string);
  return scope.Close(to);
}

Persistent<Function> GitPatch::constructor_template;

/**
 * Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * @author Michael Robinson @codeofinterest <mike@pagesofinterest.net>
 *
 * Dual licensed under the MIT and GPL licenses.
 */

#include <v8.h>
#include <node.h>

#include "cvv8/v8-convert.hpp"

#include "git2.h"

#include "../include/error.h"

using namespace v8;
using namespace node;
using namespace cvv8;

/**
 * Copied from libgit2/include/errors.h, to allow exporting to JS
 */
typedef enum {
  _GIT_OK = 0,
  _GIT_ERROR = -1,
  _GIT_ENOTFOUND = -3,
  _GIT_EEXISTS = -4,
  _GIT_EAMBIGUOUS = -5,
  _GIT_EBUFS = -6,

  _GIT_PASSTHROUGH = -30,
  _GIT_ITEROVER = -31,
} git_error_return_t;

namespace cvv8 {
  template <>
  struct NativeToJS<git_error_t> : NativeToJS<int32_t> {};

  template <>
  struct NativeToJS<git_error_return_t> : NativeToJS<int32_t> {};
}

void GitError::Initialize (Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Error"));

  // Add libgit2 error codes to error object
  Local<Object> libgit2Errors = Object::New();

  libgit2Errors->Set(String::NewSymbol("GITERR_NOMEMORY"), CastToJS(GITERR_NOMEMORY), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GITERR_OS"), CastToJS(GITERR_OS), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GITERR_INVALID"), CastToJS(GITERR_INVALID), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GITERR_REFERENCE"), CastToJS(GITERR_REFERENCE), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GITERR_ZLIB"), CastToJS(GITERR_ZLIB), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GITERR_REPOSITORY"), CastToJS(GITERR_REPOSITORY), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GITERR_CONFIG"), CastToJS(GITERR_CONFIG), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GITERR_REGEX"), CastToJS(GITERR_REGEX), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GITERR_ODB"), CastToJS(GITERR_ODB), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GITERR_INDEX"), CastToJS(GITERR_INDEX), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GITERR_OBJECT"), CastToJS(GITERR_OBJECT), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GITERR_NET"), CastToJS(GITERR_NET), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GITERR_TAG"), CastToJS(GITERR_TAG), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GITERR_TREE"), CastToJS(GITERR_TREE), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GITERR_INDEXER"), CastToJS(GITERR_INDEXER), ReadOnly);

  // Add libgit2 error codes to error object
  Local<Object> libgit2ReturnCodes = Object::New();

  libgit2ReturnCodes->Set(String::NewSymbol("GIT_OK"), CastToJS(_GIT_OK), ReadOnly);
  libgit2ReturnCodes->Set(String::NewSymbol("GIT_ERROR"), CastToJS(_GIT_ERROR), ReadOnly);
  libgit2ReturnCodes->Set(String::NewSymbol("GIT_ENOTFOUND"), CastToJS(_GIT_ENOTFOUND), ReadOnly);
  libgit2ReturnCodes->Set(String::NewSymbol("GIT_EEXISTS"), CastToJS(_GIT_EEXISTS), ReadOnly);
  libgit2ReturnCodes->Set(String::NewSymbol("GIT_EAMBIGUOUS"), CastToJS(_GIT_EAMBIGUOUS), ReadOnly);
  libgit2ReturnCodes->Set(String::NewSymbol("GIT_EBUFS"), CastToJS(_GIT_EBUFS), ReadOnly);
  libgit2ReturnCodes->Set(String::NewSymbol("GIT_PASSTHROUGH"), CastToJS(_GIT_PASSTHROUGH), ReadOnly);
  libgit2ReturnCodes->Set(String::NewSymbol("GIT_ITEROVER"), CastToJS(_GIT_ITEROVER), ReadOnly);

  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  constructor_template->Set(String::NewSymbol("codes"), libgit2Errors, ReadOnly);
  constructor_template->Set(String::NewSymbol("returnCodes"), libgit2ReturnCodes, ReadOnly);


  target->Set(String::NewSymbol("Error"), constructor_template);
}

Local<Object> GitError::WrapError(const git_error* error) {

  Local<Object> gitError = GitError::constructor_template->NewInstance();
  Local<StackTrace> stackTrace = StackTrace::CurrentStackTrace(10);

  gitError->Set(String::NewSymbol("stackTrace"), CastToJS(stackTrace->AsArray()));
  gitError->Set(String::NewSymbol("message"), String::New(error->message));
  gitError->Set(String::NewSymbol("code"), Integer::New(error->klass));

  return gitError;
}

Handle<Value> GitError::New(const Arguments& args) {
  HandleScope scope;

  GitError *error = new GitError();
  error->Wrap(args.This());

  return scope.Close(args.This());
}

Persistent<Function> GitError::constructor_template;

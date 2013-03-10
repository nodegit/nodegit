/*
 * Copyright 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * Dual licensed under the MIT and GPL licenses.
 */

#include <v8.h>
#include <node.h>

#include "cvv8/v8-convert.hpp"
#include "git2.h"

#include "../include/error.h"

using namespace v8;
using namespace cvv8;
using namespace node;

namespace cvv8 {
  template <>
  struct NativeToJS<git_error_t> : NativeToJS<int32_t> {};
}

void GitError::Initialize (Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Error"));

  // Add libgit2 error codes to error object
  Local<Object> libgit2Errors = Object::New();

  libgit2Errors->Set(String::NewSymbol("GITERR_NOMEMORY"), cvv8::CastToJS(GITERR_NOMEMORY), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GITERR_OS"), cvv8::CastToJS(GITERR_OS), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GITERR_INVALID"), cvv8::CastToJS(GITERR_INVALID), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GITERR_REFERENCE"), cvv8::CastToJS(GITERR_REFERENCE), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GITERR_ZLIB"), cvv8::CastToJS(GITERR_ZLIB), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GITERR_REPOSITORY"), cvv8::CastToJS(GITERR_REPOSITORY), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GITERR_CONFIG"), cvv8::CastToJS(GITERR_CONFIG), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GITERR_REGEX"), cvv8::CastToJS(GITERR_REGEX), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GITERR_ODB"), cvv8::CastToJS(GITERR_ODB), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GITERR_INDEX"), cvv8::CastToJS(GITERR_INDEX), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GITERR_OBJECT"), cvv8::CastToJS(GITERR_OBJECT), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GITERR_NET"), cvv8::CastToJS(GITERR_NET), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GITERR_TAG"), cvv8::CastToJS(GITERR_TAG), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GITERR_TREE"), cvv8::CastToJS(GITERR_TREE), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GITERR_INDEXER"), cvv8::CastToJS(GITERR_INDEXER), ReadOnly);

  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  constructor_template->Set(String::NewSymbol("codes"), libgit2Errors, ReadOnly);

  target->Set(String::NewSymbol("Error"), constructor_template);
}

Local<Object> GitError::WrapError(const git_error* error) {
  Local<Object> gitError = GitError::constructor_template->NewInstance();
  Local<StackTrace> stackTrace = StackTrace::CurrentStackTrace(10);
  gitError->Set(String::NewSymbol("stackTrace"), cvv8::CastToJS(stackTrace->AsArray()));
  gitError->Set(String::NewSymbol("message"), String::New(error->message));
  gitError->Set(String::NewSymbol("code"), Integer::New(error->klass));
  return gitError;
}

Handle<Value> GitError::New(const Arguments& args) {
  HandleScope scope;

  GitError *error = new GitError();
  error->Wrap(args.This());

  return scope.Close( args.This() );
}

Persistent<Function> GitError::constructor_template;

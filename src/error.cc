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
  struct NativeToJS<git_error> : NativeToJS<int32_t> {};
}

void GitError::Initialize (Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(New);

  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("Error"));

  NODE_SET_PROTOTYPE_METHOD(constructor_template, "strError", StrError);

  // Add libgit2 error codes to error object
  Local<Object> libgit2Errors = Object::New();

  libgit2Errors->Set(String::NewSymbol("GIT_SUCCESS"), cvv8::CastToJS(GIT_SUCCESS), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GIT_ERROR"), cvv8::CastToJS(GIT_ERROR), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GIT_ENOTOID"), cvv8::CastToJS(GIT_ENOTOID), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GIT_ENOTFOUND"), cvv8::CastToJS(GIT_ENOTFOUND), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GIT_ENOMEM"), cvv8::CastToJS(GIT_ENOMEM), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GIT_EOSERR"), cvv8::CastToJS(GIT_EOSERR), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GIT_EOBJTYPE"), cvv8::CastToJS(GIT_EOBJTYPE), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GIT_ENOTAREPO"), cvv8::CastToJS(GIT_ENOTAREPO), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GIT_EINVALIDTYPE"), cvv8::CastToJS(GIT_EINVALIDTYPE), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GIT_EMISSINGOBJDATA"), cvv8::CastToJS(GIT_EMISSINGOBJDATA), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GIT_EPACKCORRUPTED"), cvv8::CastToJS(GIT_EPACKCORRUPTED), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GIT_EFLOCKFAIL"), cvv8::CastToJS(GIT_EFLOCKFAIL), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GIT_EZLIB"), cvv8::CastToJS(GIT_EZLIB), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GIT_EBUSY"), cvv8::CastToJS(GIT_EBUSY), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GIT_EBAREINDEX"), cvv8::CastToJS(GIT_EBAREINDEX), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GIT_EINVALIDREFNAME"), cvv8::CastToJS(GIT_EINVALIDREFNAME), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GIT_EREFCORRUPTED"), cvv8::CastToJS(GIT_EREFCORRUPTED), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GIT_ETOONESTEDSYMREF"), cvv8::CastToJS(GIT_ETOONESTEDSYMREF), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GIT_EPACKEDREFSCORRUPTED"), cvv8::CastToJS(GIT_EPACKEDREFSCORRUPTED), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GIT_EINVALIDPATH"), cvv8::CastToJS(GIT_EINVALIDPATH), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GIT_EREVWALKOVER"), cvv8::CastToJS(GIT_EREVWALKOVER), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GIT_EINVALIDREFSTATE"), cvv8::CastToJS(GIT_EINVALIDREFSTATE), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GIT_ENOTIMPLEMENTED"), cvv8::CastToJS(GIT_ENOTIMPLEMENTED), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GIT_EEXISTS"), cvv8::CastToJS(GIT_EEXISTS), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GIT_EOVERFLOW"), cvv8::CastToJS(GIT_EOVERFLOW), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GIT_ENOTNUM"), cvv8::CastToJS(GIT_ENOTNUM), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GIT_ESTREAM"), cvv8::CastToJS(GIT_ESTREAM), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GIT_EINVALIDARGS"), cvv8::CastToJS(GIT_EINVALIDARGS), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GIT_EOBJCORRUPTED"), cvv8::CastToJS(GIT_EOBJCORRUPTED), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GIT_EAMBIGUOUSOIDPREFIX"), cvv8::CastToJS(GIT_EAMBIGUOUSOIDPREFIX), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GIT_EPASSTHROUGH"), cvv8::CastToJS(GIT_EPASSTHROUGH), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GIT_ENOMATCH"), cvv8::CastToJS(GIT_ENOMATCH), ReadOnly);
  libgit2Errors->Set(String::NewSymbol("GIT_ESHORTBUFFER"), cvv8::CastToJS(GIT_ESHORTBUFFER), ReadOnly);

  constructor_template->Set(String::NewSymbol("codes"), libgit2Errors, ReadOnly);

  target->Set(String::NewSymbol("Error"), constructor_template->GetFunction());
}

const char* GitError::StrError(int err) {
  return git_strerror(err);
}

Handle<Value> GitError::New(const Arguments& args) {
  HandleScope scope;

  GitError *error = new GitError();
  error->Wrap(args.This());

  return scope.Close( args.This() );
}

Handle<Value> GitError::StrError(const Arguments& args) {
  HandleScope scope;

  GitError* error = ObjectWrap::Unwrap<GitError>(args.This());

  if(args.Length() == 0 || !args[0]->IsNumber()) {
    return ThrowException(Exception::Error(String::New("Error is required and must be a Number.")));
  }

  Local<Integer> err = Local<Integer>::Cast(args[0]);

  return scope.Close( String::New(error->StrError(err->Value())) );
}

Persistent<FunctionTemplate> GitError::constructor_template;

/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITDELTA_H
#define GITDELTA_H

#include <nan.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class GitDelta : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_diff_delta *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitDelta(git_diff_delta *raw);
    ~GitDelta();

    static NAN_METHOD(New);

    static NAN_METHOD(OldFile);
    static NAN_METHOD(NewFile);
    static NAN_METHOD(Status);
    static NAN_METHOD(Similarity);
    static NAN_METHOD(Flags);
    git_diff_delta *raw;
};

#endif

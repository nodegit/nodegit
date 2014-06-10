/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITDIFFRANGE_H
#define GITDIFFRANGE_H

#include <nan.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class GitDiffRange : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_diff_range *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitDiffRange(git_diff_range *raw);
    ~GitDiffRange();

    static NAN_METHOD(New);

    static NAN_METHOD(OldStart);
    static NAN_METHOD(OldLines);
    static NAN_METHOD(NewStart);
    static NAN_METHOD(NewLines);
    git_diff_range *raw;
};

#endif

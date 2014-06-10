/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITDIFFOPTIONS_H
#define GITDIFFOPTIONS_H

#include <nan.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class GitDiffOptions : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_diff_options *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitDiffOptions(git_diff_options *raw);
    ~GitDiffOptions();

    static NAN_METHOD(New);

    git_diff_options *raw;
};

#endif

/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITDIFFLIST_H
#define GITDIFFLIST_H

#include <nan.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class GitDiffList : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_diff_list *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitDiffList(git_diff_list *raw);
    ~GitDiffList();

    static NAN_METHOD(New);

    static NAN_METHOD(Merge);
    static NAN_METHOD(FindSimilar);
    static NAN_METHOD(Size);
    static NAN_METHOD(NumDeltasOfType);
    static NAN_METHOD(Patch);
    git_diff_list *raw;
};

#endif

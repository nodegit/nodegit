/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITPATCH_H
#define GITPATCH_H

#include <nan.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class GitPatch : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_diff_patch *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitPatch(git_diff_patch *raw);
    ~GitPatch();

    static NAN_METHOD(New);

    static NAN_METHOD(Delta);
    static NAN_METHOD(Size);
    static NAN_METHOD(Stats);
    static NAN_METHOD(Hunk);
    static NAN_METHOD(Lines);
    static NAN_METHOD(Line);
    static NAN_METHOD(ToString);
    git_diff_patch *raw;
};

#endif

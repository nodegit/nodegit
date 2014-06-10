/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef BRANCH_H
#define BRANCH_H

#include <nan.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class Branch : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_branch *GetValue();

    static Handle<Value> New(void *raw);

  private:
    Branch(git_branch *raw);
    ~Branch();

    static NAN_METHOD(New);

    static NAN_METHOD(Create);
    static NAN_METHOD(Delete);
    static NAN_METHOD(Foreach);
    static NAN_METHOD(Move);
    static NAN_METHOD(Lookup);
    static NAN_METHOD(Name);
    static NAN_METHOD(Upstream);
    static NAN_METHOD(SetUpstream);
    static NAN_METHOD(UpstreamName);
    static NAN_METHOD(IsHead);
    static NAN_METHOD(RemoteName);
    git_branch *raw;
};

#endif

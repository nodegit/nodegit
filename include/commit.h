/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITCOMMIT_H
#define GITCOMMIT_H

#include <v8.h>
#include <node.h>
#include <string>

#include "nan.h"

#include "git2.h"

using namespace node;
using namespace v8;

class GitCommit : public ObjectWrap {
  public:

    static Persistent<FunctionTemplate> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_commit *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitCommit(git_commit *raw);
    ~GitCommit();

    static NAN_METHOD(New);

    static NAN_METHOD(Oid);
    static NAN_METHOD(MessageEncoding);
    static NAN_METHOD(Message);
    static NAN_METHOD(Time);
    static NAN_METHOD(Offset);
    static NAN_METHOD(Committer);
    static NAN_METHOD(Author);
    static NAN_METHOD(TreeId);
    static NAN_METHOD(ParentCount);
    static NAN_METHOD(ParentId);
    static NAN_METHOD(NthGenAncestor);
    git_commit *raw;
};

#endif

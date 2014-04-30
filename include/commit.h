/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITCOMMIT_H
#define GITCOMMIT_H

#include <v8.h>
#include <node.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class GitCommit : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_commit *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitCommit(git_commit *raw);
    ~GitCommit();

    static Handle<Value> New(const Arguments& args);


    static Handle<Value> Oid(const Arguments& args);
    static Handle<Value> MessageEncoding(const Arguments& args);
    static Handle<Value> Message(const Arguments& args);
    static Handle<Value> Time(const Arguments& args);
    static Handle<Value> Offset(const Arguments& args);
    static Handle<Value> Committer(const Arguments& args);
    static Handle<Value> Author(const Arguments& args);
    static Handle<Value> TreeId(const Arguments& args);
    static Handle<Value> ParentCount(const Arguments& args);
    static Handle<Value> ParentId(const Arguments& args);
    static Handle<Value> NthGenAncestor(const Arguments& args);
    git_commit *raw;
};

#endif

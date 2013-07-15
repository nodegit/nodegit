/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef BRANCH_H
#define BRANCH_H

#include <v8.h>
#include <node.h>
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

    static Handle<Value> New(const Arguments& args);


    static Handle<Value> Create(const Arguments& args);
    static Handle<Value> Delete(const Arguments& args);
    static Handle<Value> Foreach(const Arguments& args);
    static Handle<Value> Move(const Arguments& args);
    static Handle<Value> Lookup(const Arguments& args);
    static Handle<Value> Name(const Arguments& args);
    static Handle<Value> Upstream(const Arguments& args);
    static Handle<Value> SetUpstream(const Arguments& args);
    static Handle<Value> UpstreamName(const Arguments& args);
    static Handle<Value> IsHead(const Arguments& args);
    static Handle<Value> RemoteName(const Arguments& args);
    git_branch *raw;
};

#endif

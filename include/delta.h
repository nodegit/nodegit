/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITDELTA_H
#define GITDELTA_H

#include <v8.h>
#include <node.h>
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

    static Handle<Value> New(const Arguments& args);

    static Handle<Value> OldFile(const Arguments& args);
    static Handle<Value> NewFile(const Arguments& args);
    static Handle<Value> Status(const Arguments& args);
    static Handle<Value> Similarity(const Arguments& args);
    static Handle<Value> Flags(const Arguments& args);

    git_diff_delta *raw;
};

#endif

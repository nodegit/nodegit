/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITDIFFRANGE_H
#define GITDIFFRANGE_H

#include <v8.h>
#include <node.h>
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

    static Handle<Value> New(const Arguments& args);

    static Handle<Value> OldStart(const Arguments& args);
    static Handle<Value> OldLines(const Arguments& args);
    static Handle<Value> NewStart(const Arguments& args);
    static Handle<Value> NewLines(const Arguments& args);

    git_diff_range *raw;
};

#endif

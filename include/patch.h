/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITPATCH_H
#define GITPATCH_H

#include <v8.h>
#include <node.h>
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

    static Handle<Value> New(const Arguments& args);


    static Handle<Value> Delta(const Arguments& args);
    static Handle<Value> Size(const Arguments& args);
    static Handle<Value> Stats(const Arguments& args);
    static Handle<Value> Hunk(const Arguments& args);
    static Handle<Value> Lines(const Arguments& args);
    static Handle<Value> Line(const Arguments& args);
    static Handle<Value> ToString(const Arguments& args);
    git_diff_patch *raw;
};

#endif

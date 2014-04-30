/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITDIFFLIST_H
#define GITDIFFLIST_H

#include <v8.h>
#include <node.h>
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

    static Handle<Value> New(const Arguments& args);


    static Handle<Value> Merge(const Arguments& args);
    static Handle<Value> FindSimilar(const Arguments& args);
    static Handle<Value> Size(const Arguments& args);
    static Handle<Value> NumDeltasOfType(const Arguments& args);
    static Handle<Value> Patch(const Arguments& args);
    git_diff_list *raw;
};

#endif

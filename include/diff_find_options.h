/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITDIFFFINDOPTIONS_H
#define GITDIFFFINDOPTIONS_H

#include <v8.h>
#include <node.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class GitDiffFindOptions : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_diff_find_options *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitDiffFindOptions(git_diff_find_options *raw);
    ~GitDiffFindOptions();

    static Handle<Value> New(const Arguments& args);


    git_diff_find_options *raw;
};

#endif

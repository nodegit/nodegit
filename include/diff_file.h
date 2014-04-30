/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITDIFFFILE_H
#define GITDIFFFILE_H

#include <v8.h>
#include <node.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class GitDiffFile : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_diff_file *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitDiffFile(git_diff_file *raw);
    ~GitDiffFile();

    static Handle<Value> New(const Arguments& args);

    static Handle<Value> Oid(const Arguments& args);
    static Handle<Value> Path(const Arguments& args);
    static Handle<Value> Size(const Arguments& args);
    static Handle<Value> Flags(const Arguments& args);
    static Handle<Value> Mode(const Arguments& args);

    git_diff_file *raw;
};

#endif

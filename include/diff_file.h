/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITDIFFFILE_H
#define GITDIFFFILE_H

#include <nan.h>
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

    static NAN_METHOD(New);

    static NAN_METHOD(Oid);
    static NAN_METHOD(Path);
    static NAN_METHOD(Size);
    static NAN_METHOD(Flags);
    static NAN_METHOD(Mode);
    git_diff_file *raw;
};

#endif

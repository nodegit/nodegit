/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITBLOB_H
#define GITBLOB_H

#include <nan.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class GitBlob : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_blob *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitBlob(git_blob *raw);
    ~GitBlob();

    static NAN_METHOD(New);

    static NAN_METHOD(Oid);
    static NAN_METHOD(Content);
    static NAN_METHOD(Size);
    static NAN_METHOD(IsBinary);
    git_blob *raw;
};

#endif

/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITREFDB_H
#define GITREFDB_H

#include <v8.h>
#include <node.h>
#include <string>

#include "nan.h"

#include "git2.h"

using namespace node;
using namespace v8;

class GitRefDb : public ObjectWrap {
  public:

    static Persistent<FunctionTemplate> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_refdb *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitRefDb(git_refdb *raw);
    ~GitRefDb();

    static NAN_METHOD(New);

    git_refdb *raw;
};

#endif

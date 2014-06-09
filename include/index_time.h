/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITINDEXTIME_H
#define GITINDEXTIME_H

#include <nan.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class GitIndexTime : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_index_time *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitIndexTime(git_index_time *raw);
    ~GitIndexTime();

    static NAN_METHOD(New);

    static NAN_METHOD(Seconds);
    static NAN_METHOD(Nanoseconds);
    git_index_time *raw;
};

#endif

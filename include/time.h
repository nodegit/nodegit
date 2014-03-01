/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITTIME_H
#define GITTIME_H

#include <v8.h>
#include <node.h>
#include <string>

#include "nan.h"

#include "git2.h"

using namespace node;
using namespace v8;

class GitTime : public ObjectWrap {
  public:

    static Persistent<FunctionTemplate> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_time *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitTime(git_time *raw);
    ~GitTime();

    static NAN_METHOD(New);

    static NAN_METHOD(Time);
    static NAN_METHOD(Offset);

    // static Handle<Value> New(const Arguments& args);

    // static Handle<Value> Time(const Arguments& args);
    // static Handle<Value> Offset(const Arguments& args);

    git_time *raw;
};

#endif

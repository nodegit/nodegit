/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITSIGNATURE_H
#define GITSIGNATURE_H

#include <v8.h>
#include <node.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class GitSignature : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_signature *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitSignature(git_signature *raw);
    ~GitSignature();

    static Handle<Value> New(const Arguments& args);

    static Handle<Value> Name(const Arguments& args);
    static Handle<Value> Email(const Arguments& args);
    static Handle<Value> Time(const Arguments& args);

    static Handle<Value> Create(const Arguments& args);
    static Handle<Value> Now(const Arguments& args);
    git_signature *raw;
};

#endif

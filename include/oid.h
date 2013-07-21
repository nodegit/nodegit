/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITOID_H
#define GITOID_H

#include <v8.h>
#include <node.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class GitOid : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_oid *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitOid(git_oid *raw);
    ~GitOid();

    static Handle<Value> New(const Arguments& args);


    static Handle<Value> FromString(const Arguments& args);
    static Handle<Value> Sha(const Arguments& args);
    git_oid *raw;
};

#endif

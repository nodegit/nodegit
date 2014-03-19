/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITOID_H
#define GITOID_H

#include <v8.h>
#include <node.h>
#include <string>

#include "nan.h"

#include "git2.h"

using namespace node;
using namespace v8;

class GitOid : public ObjectWrap {
  public:

    static Persistent<FunctionTemplate> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_oid *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitOid(git_oid *raw);
    ~GitOid();

    static NAN_METHOD(New);
    
    static NAN_METHOD(FromString);
    static NAN_METHOD(Sha);
    
    git_oid *raw;
};

#endif

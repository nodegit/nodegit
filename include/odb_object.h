/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITODBOBJECT_H
#define GITODBOBJECT_H

#include <v8.h>
#include <node.h>
#include <string>

#include "nan.h"

#include "git2.h"

using namespace node;
using namespace v8;

class GitOdbObject : public ObjectWrap {
  public:

    static Persistent<FunctionTemplate> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_odb_object *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitOdbObject(git_odb_object *raw);
    ~GitOdbObject();

    static NAN_METHOD(New);
    
    static NAN_METHOD(Data);
    static NAN_METHOD(Size);
    static NAN_METHOD(Type);
    static NAN_METHOD(Oid);
    git_odb_object *raw;
};

#endif

/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITODBOBJECT_H
#define GITODBOBJECT_H

#include <v8.h>
#include <node.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class GitOdbObject : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_odb_object *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitOdbObject(git_odb_object *raw);
    ~GitOdbObject();

    static Handle<Value> New(const Arguments& args);


    static Handle<Value> Data(const Arguments& args);
    static Handle<Value> Size(const Arguments& args);
    static Handle<Value> Type(const Arguments& args);
    static Handle<Value> Oid(const Arguments& args);
    git_odb_object *raw;
};

#endif

/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITOBJECT_H
#define GITOBJECT_H

#include <v8.h>
#include <node.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class GitObject : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_object *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitObject(git_object *raw);
    ~GitObject();

    static Handle<Value> New(const Arguments& args);


    static Handle<Value> Oid(const Arguments& args);
    static Handle<Value> Type(const Arguments& args);
    static Handle<Value> Peel(const Arguments& args);
    static void PeelWork(uv_work_t* req);
    static void PeelAfterWork(uv_work_t* req);

    struct PeelBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_object * peeled;
      Persistent<Value> objectReference;
      const git_object * object;
      Persistent<Value> target_typeReference;
      git_otype target_type;
      Persistent<Function> callback;
    };
    git_object *raw;
};

#endif

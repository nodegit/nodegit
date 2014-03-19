/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITTREEENTRY_H
#define GITTREEENTRY_H

#include <v8.h>
#include <node.h>
#include <string>

#include "nan.h"

#include "git2.h"

using namespace node;
using namespace v8;

class GitTreeEntry : public ObjectWrap {
  public:

    static Persistent<FunctionTemplate> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_tree_entry *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitTreeEntry(git_tree_entry *raw);
    ~GitTreeEntry();

    static NAN_METHOD(New);

    static NAN_METHOD(Name);
    static NAN_METHOD(Oid);
    static NAN_METHOD(Type);
    static NAN_METHOD(filemode);
    static NAN_METHOD(GetObject);

    static void GetObjectWork(uv_work_t* req);
    static void GetObjectAfterWork(uv_work_t* req);

    struct GetObjectBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_object * object_out;
      Persistent<Value> repoReference;
      git_repository * repo;
      Persistent<Value> entryReference;
      const git_tree_entry * entry;
      Persistent<Function> callback;
    };
    git_tree_entry *raw;
};

#endif

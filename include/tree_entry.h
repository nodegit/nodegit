/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITTREEENTRY_H
#define GITTREEENTRY_H

#include <v8.h>
#include <node.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class GitTreeEntry : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_tree_entry *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitTreeEntry(git_tree_entry *raw);
    ~GitTreeEntry();

    static Handle<Value> New(const Arguments& args);


    static Handle<Value> Name(const Arguments& args);
    static Handle<Value> Oid(const Arguments& args);
    static Handle<Value> Type(const Arguments& args);
    static Handle<Value> filemode(const Arguments& args);
    static Handle<Value> GetObject(const Arguments& args);
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

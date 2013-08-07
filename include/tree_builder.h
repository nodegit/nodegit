/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITTREEBUILDER_H
#define GITTREEBUILDER_H

#include <v8.h>
#include <node.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class GitTreeBuilder : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_treebuilder *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitTreeBuilder(git_treebuilder *raw);
    ~GitTreeBuilder();

    static Handle<Value> New(const Arguments& args);


    static Handle<Value> Create(const Arguments& args);
    static Handle<Value> Clear(const Arguments& args);
    static Handle<Value> Size(const Arguments& args);
    static Handle<Value> Get(const Arguments& args);
    static Handle<Value> Insert(const Arguments& args);
    static Handle<Value> GitTreebuilderRemove(const Arguments& args);
    static Handle<Value> Write(const Arguments& args);
    static void WriteWork(uv_work_t* req);
    static void WriteAfterWork(uv_work_t* req);

    struct WriteBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_oid * id;
      Persistent<Value> repoReference;
      git_repository * repo;
      Persistent<Value> bldReference;
      git_treebuilder * bld;
      Persistent<Function> callback;
    };
    git_treebuilder *raw;
};

#endif

/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITTREE_H
#define GITTREE_H

#include <v8.h>
#include <node.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class GitTree : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_tree *GetValue();

  private:
    GitTree(git_tree *raw);
    ~GitTree();

    static Handle<Value> New(const Arguments& args);


    static Handle<Value> Lookup(const Arguments& args);
    static void LookupWork(uv_work_t* req);
    static void LookupAfterWork(uv_work_t* req);

    struct LookupBaton {
      uv_work_t request;
      const git_error* error;
      git_tree * out;
      Persistent<Value> repoReference;
      git_repository * repo;
      Persistent<Value> idReference;
      const git_oid * id;
      Persistent<Function> callback;
    };
    static Handle<Value> Oid(const Arguments& args);
    static Handle<Value> Size(const Arguments& args);
    static Handle<Value> EntryByName(const Arguments& args);
    static Handle<Value> EntryByIndex(const Arguments& args);
    static Handle<Value> EntryByOid(const Arguments& args);
    static Handle<Value> GetEntryByPath(const Arguments& args);
    static void GetEntryByPathWork(uv_work_t* req);
    static void GetEntryByPathAfterWork(uv_work_t* req);

    struct GetEntryByPathBaton {
      uv_work_t request;
      const git_error* error;
      git_tree_entry * out;
      Persistent<Value> rootReference;
      git_tree * root;
      Persistent<Value> pathReference;
      const char * path;
      Persistent<Function> callback;
    };
    git_tree *raw;
};

#endif

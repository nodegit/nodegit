/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITTREE_H
#define GITTREE_H

#include <v8.h>
#include <node.h>
#include <string>

#include "nan.h"

#include "git2.h"

using namespace node;
using namespace v8;

class GitTree : public ObjectWrap {
  public:

    static Persistent<FunctionTemplate> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_tree *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitTree(git_tree *raw);
    ~GitTree();

    static NAN_METHOD(New);

    static NAN_METHOD(Oid);
    static NAN_METHOD(Size);
    static NAN_METHOD(EntryByName);
    static NAN_METHOD(EntryByIndex);
    static NAN_METHOD(EntryByOid);
    static NAN_METHOD(GetEntry);

    static void GetEntryWork(uv_work_t* req);
    static void GetEntryAfterWork(uv_work_t* req);

    struct GetEntryBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_tree_entry * out;
      Persistent<Value> rootReference;
      git_tree * root;
      Persistent<Value> pathReference;
      const char * path;
      Persistent<Function> callback;
    };
    static NAN_METHOD(Builder);
    static NAN_METHOD(DiffTree);
    static void DiffTreeWork(uv_work_t* req);
    static void DiffTreeAfterWork(uv_work_t* req);

    struct DiffTreeBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_diff_list * diff;
      Persistent<Value> repoReference;
      git_repository * repo;
      Persistent<Value> old_treeReference;
      git_tree * old_tree;
      Persistent<Value> new_treeReference;
      git_tree * new_tree;
      Persistent<Value> optsReference;
      const git_diff_options * opts;
      Persistent<Function> callback;
    };
    static NAN_METHOD(DiffIndex);
    static void DiffIndexWork(uv_work_t* req);
    static void DiffIndexAfterWork(uv_work_t* req);

    struct DiffIndexBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_diff_list * diff;
      Persistent<Value> repoReference;
      git_repository * repo;
      Persistent<Value> old_treeReference;
      git_tree * old_tree;
      Persistent<Value> indexReference;
      git_index * index;
      Persistent<Value> optsReference;
      const git_diff_options * opts;
      Persistent<Function> callback;
    };
    static NAN_METHOD(DiffWorkDir);
    static void DiffWorkDirWork(uv_work_t* req);
    static void DiffWorkDirAfterWork(uv_work_t* req);

    struct DiffWorkDirBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_diff_list * diff;
      Persistent<Value> repoReference;
      git_repository * repo;
      Persistent<Value> old_treeReference;
      git_tree * old_tree;
      Persistent<Value> optsReference;
      const git_diff_options * opts;
      Persistent<Function> callback;
    };
    git_tree *raw;
};

#endif

/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITDIFF_H
#define GITDIFF_H

#include <v8.h>
#include <node.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class GitDiff : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_diff_list *GetValue();

  private:
    GitDiff(git_diff_list *raw);
    ~GitDiff();

    static Handle<Value> New(const Arguments& args);


    static Handle<Value> TreeToTree(const Arguments& args);
    static void TreeToTreeWork(uv_work_t* req);
    static void TreeToTreeAfterWork(uv_work_t* req);

    struct TreeToTreeBaton {
      uv_work_t request;
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
    static Handle<Value> TreeToIndex(const Arguments& args);
    static void TreeToIndexWork(uv_work_t* req);
    static void TreeToIndexAfterWork(uv_work_t* req);

    struct TreeToIndexBaton {
      uv_work_t request;
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
    static Handle<Value> IndexToWorkdir(const Arguments& args);
    static void IndexToWorkdirWork(uv_work_t* req);
    static void IndexToWorkdirAfterWork(uv_work_t* req);

    struct IndexToWorkdirBaton {
      uv_work_t request;
      const git_error* error;
      git_diff_list * diff;
      Persistent<Value> repoReference;
      git_repository * repo;
      Persistent<Value> indexReference;
      git_index * index;
      Persistent<Value> optsReference;
      const git_diff_options * opts;
      Persistent<Function> callback;
    };
    static Handle<Value> TreeToWorkdir(const Arguments& args);
    static void TreeToWorkdirWork(uv_work_t* req);
    static void TreeToWorkdirAfterWork(uv_work_t* req);

    struct TreeToWorkdirBaton {
      uv_work_t request;
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
    static Handle<Value> Merge(const Arguments& args);
    static Handle<Value> FindSimilar(const Arguments& args);
    static Handle<Value> StatusChar(const Arguments& args);
    static Handle<Value> NumDeltas(const Arguments& args);
    static Handle<Value> NumDeltasOfType(const Arguments& args);
    static Handle<Value> GetPatch(const Arguments& args);
    git_diff_list *raw;
};

#endif

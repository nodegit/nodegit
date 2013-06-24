/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITCOMMIT_H
#define GITCOMMIT_H

#include <v8.h>
#include <node.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class GitCommit : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_commit *GetValue();

  private:
    GitCommit(git_commit *raw);
    ~GitCommit();

    static Handle<Value> New(const Arguments& args);


    static Handle<Value> Lookup(const Arguments& args);
    static void LookupWork(uv_work_t* req);
    static void LookupAfterWork(uv_work_t* req);

    struct LookupBaton {
      uv_work_t request;
      const git_error* error;
      git_commit * commit;
      Persistent<Value> repoReference;
      git_repository * repo;
      Persistent<Value> idReference;
      const git_oid * id;
      Persistent<Function> callback;
    };
    static Handle<Value> Oid(const Arguments& args);
    static Handle<Value> MessageEncoding(const Arguments& args);
    static Handle<Value> Message(const Arguments& args);
    static Handle<Value> Time(const Arguments& args);
    static Handle<Value> Offset(const Arguments& args);
    static Handle<Value> Committer(const Arguments& args);
    static Handle<Value> Author(const Arguments& args);
    static Handle<Value> Tree(const Arguments& args);
    static void TreeWork(uv_work_t* req);
    static void TreeAfterWork(uv_work_t* req);

    struct TreeBaton {
      uv_work_t request;
      const git_error* error;
      git_tree * tree_out;
      Persistent<Value> commitReference;
      const git_commit * commit;
      Persistent<Function> callback;
    };
    static Handle<Value> TreeId(const Arguments& args);
    static Handle<Value> ParentCount(const Arguments& args);
    static Handle<Value> Parent(const Arguments& args);
    static void ParentWork(uv_work_t* req);
    static void ParentAfterWork(uv_work_t* req);

    struct ParentBaton {
      uv_work_t request;
      const git_error* error;
      git_commit * out;
      Persistent<Value> commitReference;
      git_commit * commit;
      Persistent<Value> nReference;
      unsigned int n;
      Persistent<Function> callback;
    };
    static Handle<Value> ParentId(const Arguments& args);
    static Handle<Value> NthGenAncestor(const Arguments& args);
    git_commit *raw;
};

#endif

/*
 * Copyright 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * Dual licensed under the MIT and GPL licenses.
 */

#ifndef COMMIT_H
#define COMMIT_H

#include <v8.h>
#include <node.h>
#include <vector>

#include "../vendor/libgit2/include/git2.h"

#include "reference.h"
#include "repo.h"
#include "oid.h"
#include "tree.h"

using namespace node;
using namespace v8;

/**
 * Class wrapper for libgit2 git_commit
 */
class GitCommit : public ObjectWrap {
  public:
    /**
     * v8::FunctionTemplate used to create Node.js constructor
     */
    static Persistent<Function> constructor_template;

    /**
     * Used to intialize the EventEmitter from Node.js
     *
     * @param target v8::Object the Node.js module object
     */
    static void Initialize (Handle<v8::Object> target);

    git_commit* GetValue();
    void SetValue(git_commit* commit);
    void Close();

    // const git_oid* Id();
    // const char* MessageShort();
    // const char* Message();
    // time_t Time();
    // int TimeOffset();
    // const git_signature* Committer();
    // const git_signature* Author();
    // int Tree(git_tree** tree);
    // unsigned int ParentCount();
    // int Parent(git_commit** commit, int pos);

  protected:
    GitCommit() {}
    ~GitCommit() {}

    static Handle<Value> New(const Arguments& args);
    static Handle<Value> NewInstance();

    static Handle<Value> FetchDetails(const Arguments& args);
    static void FetchDetailsWork(uv_work_t *req);
    static void FetchDetailsAfterWork(uv_work_t *req);

    static Handle<Value> Lookup(const Arguments& args);
    static void LookupWork(uv_work_t *req);
    static void LookupAfterWork(uv_work_t *req);

    static Handle<Value> Close(const Arguments& args);
    static Handle<Value> Id(const Arguments& args);
    static Handle<Value> MessageShort(const Arguments& args);
    static Handle<Value> Message(const Arguments& args);
    static Handle<Value> Time(const Arguments& args);
    static Handle<Value> TimeOffset(const Arguments& args);
    static Handle<Value> Committer(const Arguments& args);
    static Handle<Value> Author(const Arguments& args);

    static Handle<Value> Tree(const Arguments& args);
    static void TreeWork(uv_work_t* req);
    static void TreeAfterWork(uv_work_t* req);

    static Handle<Value> ParentCount(const Arguments& args);

    static Handle<Value> ParentSync(const Arguments& args);
    static Handle<Value> Parent(const Arguments& args);
    static void ParentWork(uv_work_t* req);
    static void ParentAfterWork(uv_work_t* req);

  private:
    git_commit* commit;
    git_oid* oid;

    struct lookup_request {
      GitCommit* commit;
      GitRepo* repo;
      GitOid* oid;
      int err;
      Persistent<Function> callback;
    };

    /**
     * Struct containing details for a commit.
     */
    struct FetchDetailsBaton {
      uv_work_t request;
      int errorCode;
      const char* errorMessage;

      git_commit* rawCommit;

      const git_oid *oid;
      char sha[GIT_OID_HEXSZ + 1];
      const char* message;
      time_t time;
      int timeOffset;
      const git_signature* committer;
      const git_signature* author;
      unsigned int parentCount;
      std::vector<std::string> parentShas;

      Persistent<Function> callback;
    };

    /**
     * Contains references to the current commit, parent commit (output)
     * parent commit's index, also contains references to an error code post
     * lookup, and a callback function to execute.
     */
    struct ParentBaton {
      uv_work_t request;
      int errorCode;
      const char* errorMessage;

      int index;
      GitCommit* commit;
      git_commit* rawParentCommit;

      Persistent<Function> callback;
    };

};
#endif

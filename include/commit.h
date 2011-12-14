/*
 * Copyright 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * Dual licensed under the MIT and GPL licenses.
 */

#ifndef COMMIT_H
#define COMMIT_H

#include <v8.h>
#include <node.h>

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
    static Persistent<FunctionTemplate> constructor_template;

    /**
     * Used to intialize the EventEmitter from Node.js
     *
     * @param target v8::Object the Node.js module object
     */
    static void Initialize (Handle<v8::Object> target);

    git_commit* GetValue();
    void SetValue(git_commit* commit);
    int Lookup(git_repository* repo, git_oid* oid);
    void Close();
    const git_oid* Id();
    const char* MessageShort();
    const char* Message();
    time_t Time();
    int TimeOffset();
    const git_signature* Committer();
    const git_signature* Author();
    int Tree(git_tree** tree);
    unsigned int ParentCount();
    int Parent(git_commit** commit, int pos);

  protected:
    GitCommit() {}
    ~GitCommit() {}

    static Handle<Value> New(const Arguments& args);

    static Handle<Value> Lookup(const Arguments& args);
    static void EIO_Lookup(eio_req *req);
    static int EIO_AfterLookup(eio_req *req);

    static Handle<Value> Close(const Arguments& args);
    static Handle<Value> Id(const Arguments& args);
    static Handle<Value> MessageShort(const Arguments& args);
    static Handle<Value> Message(const Arguments& args);
    static Handle<Value> Time(const Arguments& args);
    static Handle<Value> TimeOffset(const Arguments& args);
    static Handle<Value> Committer(const Arguments& args);
    static Handle<Value> Author(const Arguments& args);

    static Handle<Value> Tree(const Arguments& args);
    static int EIO_Tree(eio_req* req);
    static int EIO_AfterTree(eio_req* req);

    static Handle<Value> ParentCount(const Arguments& args);
    static Handle<Value> Parent(const Arguments& args);

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

    //struct tree_request {
    //  GitCommit* commit;
    //  Tree* tree;
    //  Persistent<Function> callback;
    //};
};
#endif

/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#ifndef REVWALK_H
#define REVWALK_H

#include <v8.h>
#include <node.h>

#include "../vendor/libgit2/include/git2.h"

#include "repo.h"
#include "commit.h"

using namespace node;
using namespace v8;

class GitRevWalk : public ObjectWrap {
  public:
    static Persistent<FunctionTemplate> constructor_template;
    static void Initialize(Handle<v8::Object> target);

    git_revwalk* GetValue();
    void SetValue(git_revwalk* revwalk);
    int New(git_repository* repo);
    void Reset();
    int Push(git_oid* oid);
    int Hide();
    int Next(git_oid* oid);
    int Sorting(int sort);
    void Free();
    git_repository* Repository();

  protected:
    GitRevWalk() {}
    ~GitRevWalk() {}
    static Handle<Value> New(const Arguments& args);
    static Handle<Value> Reset(const Arguments& args);
    static Handle<Value> Push(const Arguments& args);
    static Handle<Value> Hide(const Arguments& args);

    static Handle<Value> Next(const Arguments& args);
    static void EIO_Next(eio_req* req);
    static int EIO_AfterNext(eio_req* req);

    static Handle<Value> Sorting(const Arguments& args);
    static Handle<Value> Free(const Arguments& args);
    static Handle<Value> Repository(const Arguments& args);

  private:
    git_revwalk* revwalk;
    git_repository* repo;

    struct next_request {
      GitRevWalk* revwalk;
      GitOid* oid;
      int err;
      Persistent<Function> callback;
    };
};

#endif

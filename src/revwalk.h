/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#ifndef REVWALK_H
#define REVWALK_H

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include "../vendor/libgit2/src/git2.h"

#include "repo.h"
#include "commit.h"

using namespace node;
using namespace v8;

class RevWalk : public EventEmitter {
  public:
    static Persistent<FunctionTemplate> constructor_template;
    static void Initialize(Handle<v8::Object> target);

    git_revwalk* GetValue();
    void SetValue(git_revwalk* revwalk);
    int New(Repo *repo);
    void Reset();
    int Push(Commit *commit);
    int Hide();
    int Next(git_commit** commit);
    int Sorting(int sort);
    void Free();
    git_repository* Repository();

  protected:
    RevWalk() {}
    ~RevWalk() {}
    static Handle<Value> New(const Arguments& args);
    static Handle<Value> Reset(const Arguments& args);
    static Handle<Value> Push(const Arguments& args);
    static Handle<Value> Hide(const Arguments& args);

    static Handle<Value> Next(const Arguments& args);
    static int EIO_Next(eio_req *req);
    static int EIO_AfterNext(eio_req *req);

    static Handle<Value> Sorting(const Arguments& args);
    static Handle<Value> Free(const Arguments& args);
    static Handle<Value> Repository(const Arguments& args);

  private:
    git_revwalk *revwalk;

    struct next_request {
      RevWalk *revwalk;
      Commit *commit;
      Persistent<Value> err;
      Persistent<Function> callback;
    };
};

#endif

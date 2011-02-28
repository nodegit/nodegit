/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#ifndef REVWALK_H
#define REVWALK_H

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include <git2.h>

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
    int Push(Commit *commit);
    int Next(Commit *commit);
    void Free();

    //void 	git_revwalk_reset (git_revwalk *walker)
    //int 	git_revwalk_push (git_revwalk *walk, git_commit *commit)
    //int 	git_revwalk_hide (git_revwalk *walk, git_commit *commit)
    //int 	git_revwalk_next (git_commit **commit, git_revwalk *walk)
    //int 	git_revwalk_sorting (git_revwalk *walk, unsigned int sort_mode)
    //void 	git_revwalk_free (git_revwalk *walk)
    //git_repository * 	git_revwalk_repository (git_revwalk *walk)

  protected:
    RevWalk() {}
    ~RevWalk() {}
    static Handle<Value> New(const Arguments& args);
    static Handle<Value> Push(const Arguments& args);

    static Handle<Value> Next(const Arguments& args);
    static int EIO_Next(eio_req *req);
    static int EIO_AfterNext(eio_req *req);

    static Handle<Value> Free(const Arguments& args);

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

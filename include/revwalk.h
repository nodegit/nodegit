/*
 * Copyright 2013, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * @author Michael Robinson @codeofinterest <mike@pagesofinterest.net>
 *
 * Dual licensed under the MIT and GPL licenses.
 */
#ifndef REVWALK_H
#define REVWALK_H

#include <v8.h>
#include <node.h>

#include "../vendor/libgit2/include/git2.h"

#include "repo.h"

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
    int Sorting(int sort);
    void Free();
    git_repository* Repository();

  protected:
    GitRevWalk() {}
    ~GitRevWalk() {}
    static Handle<Value> New(const Arguments& args);
    static Handle<Value> Reset(const Arguments& args);

    /**
     * Although git_revwalk_next is not blocking when iterating with a
     * time-sorting mode, options may be added later to allow different sort
     * modes, hence the async implementation.
     */
    static Handle<Value> Push(const Arguments& args);
    static void PushWork(uv_work_t *req);
    static void PushAfterWork(uv_work_t *req);

    static Handle<Value> Hide(const Arguments& args);

    static Handle<Value> Next(const Arguments& args);
    static void NextWork(uv_work_t* req);
    static void NextAfterWork(uv_work_t* req);

    static Handle<Value> Sorting(const Arguments& args);
    static Handle<Value> Free(const Arguments& args);
    static Handle<Value> Repository(const Arguments& args);

  private:
    git_revwalk* revwalk;
    git_repository* repo;

    struct PushBaton {
      uv_work_t request;
      const git_error* error;

      git_revwalk *revwalk;
      git_oid oid;

      Persistent<Function> callback;
    };

    struct NextBaton {
      uv_work_t request;
      const git_error* error;
      bool walkOver;

      git_revwalk *revwalk;
      git_oid oid;

      Persistent<Function> callback;
    };
};

#endif

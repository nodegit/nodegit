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

#include "git2.h"

#include "repo.h"

using namespace node;
using namespace v8;

class GitRevWalk : public ObjectWrap {
  public:
    static Persistent<Function> constructor_template;
    static void Initialize(Handle<v8::Object> target);

    git_revwalk* GetValue();
    void SetValue(git_revwalk* revwalk);
    git_repository* GetRepo();
    void SetRepo(git_repository* repository);

  protected:
    GitRevWalk() {}
    ~GitRevWalk() {}

    static Handle<Value> New(const Arguments& args);
    static Handle<Value> Free(const Arguments& args);

    static Handle<Value> Reset(const Arguments& args);

    static Handle<Value> Allocate(const Arguments& args);
    static void AllocateWork(uv_work_t *req);
    static void AllocateAfterWork(uv_work_t *req);

    static Handle<Value> Push(const Arguments& args);
    static void PushWork(uv_work_t *req);
    static void PushAfterWork(uv_work_t *req);

    static Handle<Value> Next(const Arguments& args);
    static void NextWork(uv_work_t* req);
    static void NextAfterWork(uv_work_t* req);

    static Handle<Value> Sorting(const Arguments& args);

  private:
    git_revwalk* revwalk;
    git_repository* repo;

    struct AllocateBaton {
      uv_work_t request;
      const git_error* error;

      GitRevWalk* revwalk;
      git_revwalk *rawRevwalk;
      git_repository* rawRepo;

      Persistent<Function> callback;
    };

    struct PushBaton {
      uv_work_t request;
      const git_error* error;

      git_revwalk *rawRevwalk;
      git_oid rawOid;

      Persistent<Function> callback;
    };

    struct NextBaton {
      uv_work_t request;

      const git_error* error;
      bool walkOver;

      git_revwalk *rawRevwalk;
      git_oid rawOid;

      Persistent<Function> callback;
    };
};

#endif

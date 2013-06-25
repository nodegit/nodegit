/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITREPO_H
#define GITREPO_H

#include <v8.h>
#include <node.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class GitRepo : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_repository *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitRepo(git_repository *raw);
    ~GitRepo();

    static Handle<Value> New(const Arguments& args);


    static Handle<Value> Open(const Arguments& args);
    static void OpenWork(uv_work_t* req);
    static void OpenAfterWork(uv_work_t* req);

    struct OpenBaton {
      uv_work_t request;
      const git_error* error;
      git_repository * out;
      Persistent<Value> pathReference;
      const char * path;
      Persistent<Function> callback;
    };
    static Handle<Value> Init(const Arguments& args);
    static void InitWork(uv_work_t* req);
    static void InitAfterWork(uv_work_t* req);

    struct InitBaton {
      uv_work_t request;
      const git_error* error;
      git_repository * out;
      Persistent<Value> pathReference;
      const char * path;
      Persistent<Value> is_bareReference;
      unsigned is_bare;
      Persistent<Function> callback;
    };
    static Handle<Value> Path(const Arguments& args);
    static Handle<Value> Workdir(const Arguments& args);
    git_repository *raw;
};

#endif

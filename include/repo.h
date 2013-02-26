/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#ifndef REPO_H
#define REPO_H

#include <v8.h>
#include <node.h>
#include <string>

#include "../vendor/libgit2/include/git2.h"

#include "object.h"

using namespace node;
using namespace v8;

class GitRepo : public ObjectWrap {
  public:
    static Persistent<FunctionTemplate> constructor_template;
    static void Initialize(Handle<v8::Object> target);
    git_repository* GetValue();
    void SetValue(git_repository* repo);
    // Asynchronous
    int Open(const char* path);
    int Init(const char* path, bool is_bare);
    // Synchronous
    void Free();

    // TODO: Implement these methods
    //int Open2(const char* path);
    //int Open3(const char* path);
    //int Lookup(Object **obj, Oid *id, Otype type);
    //Odb Database();
    //int Index(Index **index);
    //int NewObject(git_object **obj, Otype type);

  protected:
    GitRepo() {}
    ~GitRepo() {}
    static Handle<Value> New(const Arguments& args);

    static Handle<Value> Open(const Arguments& args);
    static void EIO_Open(uv_work_t* req);
    static void EIO_AfterOpen(uv_work_t* req);

    static Handle<Value> Lookup(const Arguments& args);
    static void EIO_Lookup(uv_work_t* req);
    static void EIO_AfterLookup(uv_work_t* req);

    static Handle<Value> Free(const Arguments& args);

    static Handle<Value> Init(const Arguments& args);
    static void EIO_Init(uv_work_t* req);
    static void EIO_AfterInit(uv_work_t* req);

  private:
    git_repository* repo;

    struct open_request {
      GitRepo* repo;
      int err;
      std::string path;
      Persistent<Function> callback;
    };

    struct lookup_request {
      GitRepo* repo;
      Persistent<Function> callback;
    };

    struct init_request {
      GitRepo* repo;
      int err;
      std::string path;
      bool is_bare;
      Persistent<Function> callback;
    };
};

#endif

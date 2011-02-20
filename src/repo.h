/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#ifndef REPO_H
#define REPO_H

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include <git2.h>

#include "ref.h"

using namespace node;
using namespace v8;

class Repo : public EventEmitter {
  public:
    static Persistent<FunctionTemplate> constructor_template;
    static void Initialize(Handle<v8::Object> target);
    git_repository* GetValue();
    int Open(const char* path);
    void Free();
    int Init(const char* path, bool is_bare);
    int LookupRef(git_reference* ref, const char* name);

    // TODO: Implement these methods
    //int Open2(const char* path);
    //int Open3(const char* path);
    //int Lookup(Object **obj, Oid *id, Otype type);
    //Odb Database();
    //int Index(Index **index);
    //int NewObject(git_object **obj, Otype type);

  protected:
    Repo() {}
    ~Repo() {}
    static Handle<Value> New(const Arguments& args);

    static Handle<Value> Open(const Arguments& args);
    static int EIO_Open(eio_req *req);
    static int EIO_AfterOpen(eio_req *req);

    static Handle<Value> Free(const Arguments& args);

    static Handle<Value> Init(const Arguments& args);
    static int EIO_Init(eio_req *req);
    static int EIO_AfterInit(eio_req *req);

    static Handle<Value> LookupRef(const Arguments& args);
    static int EIO_LookupRef(eio_req *req);
    static int EIO_AfterLookupRef(eio_req *req);

  private:
    git_repository *repo;
};

struct open_request {
  Repo *repo;
  Persistent<Value> err;
  Persistent<Value> path;
  Persistent<Function> callback;
};

struct init_request {
  Repo *repo;
  Persistent<Value> err;
  Persistent<Value> path;
  Persistent<Boolean> is_bare;
  Persistent<Function> callback;
};

struct lookupref_request {
  Repo *repo;
  Ref *ref;
  Persistent<Value> err;
  Persistent<Value> name;
  Persistent<Function> callback;
};

#endif

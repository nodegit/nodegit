/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#ifndef COMMIT_H
#define COMMIT_H

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include <git2.h>

#include "repo.h"
#include "oid.h"

using namespace node;
using namespace v8;

class Commit : public EventEmitter {
  public:
    static Persistent<FunctionTemplate> constructor_template;
    static void Initialize (Handle<v8::Object> target);
    git_commit* GetValue();
    int Lookup(Repo *repo, Oid *oid);

  protected:
    Commit() {}
    ~Commit() {}
    static Handle<Value> New(const Arguments& args);

    static Handle<Value> Lookup(const Arguments& args);
    static int EIO_Lookup(eio_req *req);
    static int EIO_AfterLookup(eio_req *req);

  private:
    git_commit *commit;
};

struct lookup_request {
  Commit *commit;
  Repo *repo;
  Oid *oid;
  Persistent<Value> err;
  Persistent<Function> callback;
};

#endif

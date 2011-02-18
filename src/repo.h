/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#ifndef REPO_H
#define REPO_H

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include <git2.h>

using namespace node;
using namespace v8;

class Repo : public EventEmitter {
  public:
    static Persistent<FunctionTemplate> constructor_template;
    static void Init (Handle<Object> target);
    int Open(const char* path);
    int Free();
  protected:
    Repo() {}
    ~Repo() {}
    static Handle<Value> New (const Arguments& args);
    static Handle<Value> Open (const Arguments& args);
    static Handle<Value> Free (const Arguments& args);

    static int AsyncOpen(eio_req *req);
    static int AsyncOpenComplete(eio_req *req);

  private:
    git_repository *repo_;
};

struct open_request {
  Repo *repo;
  Persistent<Value> err;
  Persistent<Value> path;
  Persistent<Function> callback;
};

#endif

/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#ifndef BLOB_H
#define BLOB_H

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include <git2.h>

#include "repo.h"

using namespace v8;
using namespace node;

class Blob : public EventEmitter {
  public:
    static Persistent<FunctionTemplate> constructor_template;
    static void Initialize(Handle<v8::Object> target);

    int New(git_repository *repo);
    git_blob* GetValue();
    void SetValue(git_blob* blob);

  protected:
    Blob() {};
    ~Blob() {};

    static Handle<Value> New(const Arguments& args);

  private:
    git_blob *blob;
};

#endif

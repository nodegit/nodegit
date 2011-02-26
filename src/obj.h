/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#ifndef OBJ_H
#define OBJ_H

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include <git2.h>

#include "repo.h"

using namespace node;

class Obj : public EventEmitter {
  public:
    static Persistent<FunctionTemplate> constructor_template;
    static void Initialize(Handle<v8::Object> target);
    // Synchronous
    int New();
    git_object* GetValue();
    void SetValue(git_object* obj);

  protected:
    Obj() {};
    ~Obj() {};

    static Handle<Value> New(const Arguments& args);

  private:
    git_object *obj;
};

#endif

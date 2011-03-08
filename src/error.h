/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#ifndef ERROR_H
#define ERROR_H

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include "../vendor/libgit2/src/git2.h"

using namespace v8;
using namespace node;

class Error : public EventEmitter {
  public:
    static Persistent<FunctionTemplate> constructor_template;
    static void Initialize(Handle<v8::Object> target);

  protected:
    Error() {};
    ~Error() {};

    static Handle<Value> New(const Arguments& args);

    static Handle<Value> StrError(const Arguments& args);
};

#endif

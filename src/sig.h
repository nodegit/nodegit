/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#ifndef Sig_H
#define Sig_H

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include <git2.h>

#include "repo.h"

using namespace v8;
using namespace node;

class Sig : public EventEmitter {
  public:
    static Persistent<FunctionTemplate> constructor_template;
    static void Initialize(Handle<v8::Object> target);
    // Synchronous
    void New(const char *name, const char *email, time_t time, int offset);
    git_signature* GetValue();
    void SetValue(git_signature* Sig);
    void Free();

  protected:
    Sig() {};
    ~Sig() {};

    static Handle<Value> New(const Arguments& args);
    static Handle<Value> Free(const Arguments& args);

  private:
    git_signature* sig;
};

#endif

/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#ifndef REF_H
#define REF_H

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include "../vendor/libgit2/src/git2.h"

#include "oid.h"

using namespace node;
using namespace v8;

class Reference : public EventEmitter {
  public:
    static Persistent<FunctionTemplate> constructor_template;
    static void Initialize(Handle<v8::Object> target);
    git_reference* GetValue();
    int New(git_repository* repo);
    void SetValue(git_reference* ref);
    const git_oid* _Oid();

  protected:
    Reference() {}
    ~Reference() {}
    static Handle<Value> New(const Arguments& args);

    static Handle<Value> _Oid(const Arguments& args);

  private:
    git_reference *ref;
};

#endif

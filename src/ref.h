/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#ifndef REF_H
#define REF_H

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include <git2.h>

using namespace node;
using namespace v8;

class Ref : public EventEmitter {
  public:
    static Persistent<FunctionTemplate> constructor_template;
    static void Initialize (Handle<v8::Object> target);
    git_reference* GetValue();
    void SetValue(git_reference* ref);

  protected:
    Ref() {}
    ~Ref() {}
    static Handle<Value> New(const Arguments& args);

  private:
    git_reference *ref;
};

#endif

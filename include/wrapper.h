/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef WRAPPER_H
#define WRAPPER_H

#include <v8.h>
#include <node.h>

using namespace node;
using namespace v8;

class Wrapper : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    void *GetValue();
    static Handle<Value> New(void *raw);

  private:
    Wrapper(void *raw);

    static Handle<Value> New(const Arguments& args);
    static Handle<Value> ToBuffer(const Arguments& args);

    void *raw;
};

#endif

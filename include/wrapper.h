/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef WRAPPER_H
#define WRAPPER_H

#include <v8.h>
#include <node.h>

#include "nan.h"

using namespace node;
using namespace v8;

class Wrapper : public ObjectWrap {
  public:

    static Persistent<FunctionTemplate> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    void *GetValue();
    static Handle<Value> New(void *raw);

  private:
    Wrapper(void *raw);

    static NAN_METHOD(New);
    static NAN_METHOD(ToBuffer);

    void *raw;
};

#endif

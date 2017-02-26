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

class Wrapper : public Nan::ObjectWrap {
  public:

    static Nan::Persistent<FunctionTemplate> constructor_template;
    static void InitializeComponent (v8::Local<v8::Object> target);

    void *GetValue();
    static v8::Local<v8::Value> New(const void *raw);

  private:
    Wrapper(void *raw);

    static NAN_METHOD(JSNewFunction);
    static NAN_METHOD(ToBuffer);

    void *raw;
};

#endif

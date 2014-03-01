/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITTHREADS_H
#define GITTHREADS_H

#include <v8.h>
#include <node.h>
#include <string>

#include "nan.h"

#include "git2.h"

using namespace node;
using namespace v8;

class GitThreads : public ObjectWrap {
  public:

    static Persistent<FunctionTemplate> constructor_template;
    static void Initialize (Handle<v8::Object> target);


  private:
    static NAN_METHOD(New);

    static NAN_METHOD(Init);
    static NAN_METHOD(Shutdown);
};

#endif

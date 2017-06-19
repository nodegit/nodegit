// This is a generated file, modify: generate/templates/templates/class_header.h

#ifndef GITFILTERREGISTRY_H
#define GITFILTERREGISTRY_H
#include <nan.h>
#include <string>
#include <queue>
#include <utility>

#include "async_baton.h"
#include "nodegit_wrapper.h"
#include "promise_completion.h"

extern "C" {
#include <git2.h>
}

#include "../include/typedefs.h"

#include "../include/filter.h"

using namespace node;
using namespace v8;


class GitFilterRegistry : public
  Nan::ObjectWrap
{
   public:
    static void InitializeComponent (v8::Local<v8::Object> target);

    static Nan::Persistent<v8::Object> persistentHandle;

  private:
         
    static NAN_METHOD(GitFilterRegister);

    static NAN_METHOD(GitFilterUnregister);
};

#endif

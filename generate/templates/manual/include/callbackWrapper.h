#ifndef CALLBACK_WRAPPER_H
#define CALLBACK_WRAPPER_H

#include <v8.h>
#include <node.h>

#include "nan.h"

using namespace v8;
using namespace node;

struct CallbackWrapper {
  NanCallback* jsCallback;
  void * payload;
};

#endif

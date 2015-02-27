#ifndef PAYLOAD_WRAPPER_H
#define PAYLOAD_WRAPPER_H

#include <v8.h>
#include <node.h>

#include "nan.h"

using namespace v8;
using namespace node;

struct PayloadWrapper {
  NanCallback* jsCallback;
  uv_async_t handle;
};

#endif

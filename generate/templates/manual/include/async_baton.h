#ifndef ASYNC_BATON
#define ASYNC_BATON

#include <uv.h>
#include <nan.h>

// Base class for Batons used for callbacks (for example,
// JS functions passed as callback parameters,
// or field properties of configuration objects whose values are callbacks)
struct AsyncBaton {
  uv_async_t req;

  bool done;
};

#endif

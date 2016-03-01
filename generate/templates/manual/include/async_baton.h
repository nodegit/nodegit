#ifndef ASYNC_BATON
#define ASYNC_BATON

#include <uv.h>
#include <nan.h>

#include "lock_master.h"
#include "functions/sleep_for_ms.h"

// Base class for Batons used for callbacks (for example,
// JS functions passed as callback parameters,
// or field properties of configuration objects whose values are callbacks)
struct AsyncBaton {
  uv_async_t req;

  bool done;
};

template<typename ResultT>
struct AsyncBatonWithResult : public AsyncBaton {
  ResultT result;
  ResultT defaultResult; // result returned if the callback doesn't return anything valid

  AsyncBatonWithResult(const ResultT &defaultResult)
    : defaultResult(defaultResult) {
  }

  ResultT ExecuteAsync(uv_async_cb asyncCallback) {
    result = 0;
    req.data = this;
    done = false;

    uv_async_init(uv_default_loop(), &req, asyncCallback);
    {
      LockMaster::TemporaryUnlock temporaryUnlock;

      uv_async_send(&req);

      while(!done) {
        sleep_for_ms(1);
      }
    }

    return result;
  }
};

#endif

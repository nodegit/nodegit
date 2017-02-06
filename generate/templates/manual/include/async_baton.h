#ifndef ASYNC_BATON
#define ASYNC_BATON

#include <uv.h>
#include <nan.h>

#include "lock_master.h"
#include "nodegit.h"

// Base class for Batons used for callbacks (for example,
// JS functions passed as callback parameters,
// or field properties of configuration objects whose values are callbacks)
struct AsyncBaton {
  uv_sem_t semaphore;
};

template<typename ResultT>
struct AsyncBatonWithResult : public AsyncBaton {
  ResultT result;
  ResultT defaultResult; // result returned if the callback doesn't return anything valid

  AsyncBatonWithResult(const ResultT &defaultResult)
    : defaultResult(defaultResult) {
    uv_sem_init(&semaphore, 0);
  }

  ~AsyncBatonWithResult() {
    uv_sem_destroy(&semaphore);
  }

  void Done() {
    // signal completion
    uv_sem_post(&semaphore);
  }

  ResultT ExecuteAsync(ThreadPool::Callback asyncCallback) {
    result = 0;

    {
      LockMaster::TemporaryUnlock temporaryUnlock;

      libgit2ThreadPool.ExecuteReverseCallback(asyncCallback, this);

      // wait for completion
      uv_sem_wait(&semaphore);
    }

    return result;
  }
};

#endif

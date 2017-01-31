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

  virtual ~AsyncBaton() {}
};

void deleteBaton(AsyncBaton *baton);

template<typename ResultT>
struct AsyncBatonWithResult : public AsyncBaton {
  ResultT result;
  ResultT defaultResult; // result returned if the callback doesn't return anything valid
  void (*onCompletion)(AsyncBaton *);

  AsyncBatonWithResult(const ResultT &defaultResult)
    : defaultResult(defaultResult) {
  }

  void Done() {
    if (onCompletion) {
      onCompletion(this);
    } else {
      // signal completion
      uv_sem_post(&semaphore);
    }
  }

  ResultT ExecuteAsync(ThreadPool::Callback asyncCallback, void (*onCompletion)(AsyncBaton *) = NULL) {
    result = 0;
    this->onCompletion = onCompletion;
    if (!onCompletion) {
      uv_sem_init(&semaphore, 0);
    }

    {
      LockMaster::TemporaryUnlock temporaryUnlock;

      libgit2ThreadPool.ExecuteReverseCallback(asyncCallback, this);

      if (!onCompletion) {
        // wait for completion
        uv_sem_wait(&semaphore);
        uv_sem_destroy(&semaphore);
      }
    }

    return result;
  }
};

#endif

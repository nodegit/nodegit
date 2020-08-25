#ifndef NODEGIT_ASYNC_WORKER
#define NODEGIT_ASYNC_WORKER

#include <nan.h>
#include "lock_master.h"

namespace nodegit {
  class AsyncWorker : public Nan::AsyncWorker {
  public:
    AsyncWorker(Nan::Callback *callback, const char *resourceName);

    virtual nodegit::LockMaster AcquireLocks() = 0;

    Nan::AsyncResource *GetAsyncResource();
  };
}

#endif

#ifndef NODEGIT_ASYNC_WORKER
#define NODEGIT_ASYNC_WORKER

#include <nan.h>
#include "lock_master.h"

namespace nodegit {
  class AsyncWorker : public Nan::AsyncWorker {
  public:
    AsyncWorker(Nan::Callback *callback, const char *resourceName);

    // This must be implemented by every async worker
    // so that the thread pool can lock separately
    // from the execute method in the AsyncWorker
    virtual nodegit::LockMaster AcquireLocks() = 0;

    // Ensure that the `HandleErrorCallback` will be called
    // when the AsyncWork is complete
    void Cancel();

    // Retrieves the async resource attached to this AsyncWorker
    // This is used to inform libgit2 callbacks what asyncResource
    // they should use when working with any javascript
    Nan::AsyncResource *GetAsyncResource();
  };
}

#endif

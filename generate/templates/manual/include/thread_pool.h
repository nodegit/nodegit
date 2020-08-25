#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <functional>
#include <memory>
#include <nan.h>
#include <uv.h>

#include "async_worker.h"

namespace nodegit {
  class ThreadPoolImpl;

  class ThreadPool {
    public:
      typedef std::function<void()> Callback;
      typedef std::function<void(Callback)> QueueCallbackFn;
      typedef std::function<Callback(QueueCallbackFn, Callback)> OnPostCallbackFn;

      // Initializes thread pool and spins up the requested number of threads
      // The provided loop will be used for completion callbacks, whenever
      // queued work is completed
      ThreadPool(int numberOfThreads, uv_loop_t *loop);

      ~ThreadPool();

      // Queues work on the thread pool, followed by completion call scheduled
      // on the loop provided in the constructor.
      // QueueWork should be called on the loop provided in the constructor.
      void QueueWorker(nodegit::AsyncWorker *worker);

      // When an AsyncWorker is being executed, the threads involved in executing
      // will ensure that this is set to the AsyncResource belonging to the AsyncWorker.
      // This ensures that any callbacks from libgit2 take the correct AsyncResource
      // when scheduling work on the JS thread.
      static Nan::AsyncResource *GetCurrentAsyncResource();

      // Queues a callback on the loop provided in the constructor
      static void PostCallbackEvent(OnPostCallbackFn onPostCallback);

      // Called once at libgit2 initialization to setup contracts with libgit2
      static void InitializeGlobal();

    private:
      std::unique_ptr<ThreadPoolImpl> impl;
  };
}

#endif

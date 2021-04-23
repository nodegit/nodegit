#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <functional>
#include <memory>
#include <nan.h>
#include <uv.h>

#include "async_worker.h"

namespace nodegit {
  class Context;
  class AsyncContextCleanupHandle;
  class ThreadPoolImpl;

  class ThreadPool {
    public:
      typedef std::function<void()> Callback;
      typedef std::function<void(Callback, Callback)> QueueCallbackFn;
      typedef std::function<Callback(QueueCallbackFn, Callback)> OnPostCallbackFn;

      // Initializes thread pool and spins up the requested number of threads
      // The provided loop will be used for completion callbacks, whenever
      // queued work is completed
      ThreadPool(int numberOfThreads, uv_loop_t *loop, nodegit::Context *context);

      ThreadPool(const ThreadPool &) = delete;
      ThreadPool(ThreadPool &&) = delete;
      ThreadPool &operator=(const ThreadPool &) = delete;
      ThreadPool &operator=(ThreadPool &&) = delete;

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

      // Same as GetCurrentAsyncResource, except used to ensure callbacks occur
      // in the correct context.
      static const nodegit::Context *GetCurrentContext();

      // Same as GetCurrentAsyncResource, except used for callbacks to store errors
      // for use after completion of async work
      static Nan::Global<v8::Value> *GetCurrentCallbackErrorHandle();

      // Queues a callback on the loop provided in the constructor
      static void PostCallbackEvent(OnPostCallbackFn onPostCallback);

      // Called once at libgit2 initialization to setup contracts with libgit2
      static void InitializeGlobal();

      // Will asynchronously shutdown the thread pool
      // It will also clean up any resources that the thread pool is keeping alive
      void Shutdown(std::unique_ptr<AsyncContextCleanupHandle> cleanupHandle);

    private:
      std::unique_ptr<ThreadPoolImpl> impl;
  };
}

#endif

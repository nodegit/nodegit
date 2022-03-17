#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <functional>
#include <memory>
#include <nan.h>
#include <uv.h>

#include "async_worker.h"

// Temporary workaround for LFS checkout. Comment added to be reverted.
// With the threadpool rewrite, a Worker will execute its callbacks with
// objects temporary unlock (to prevent deadlocks), and we'll wait until
// the callback is done to lock them back again (to make sure it's thread-safe).
// LFS checkout lost performance after this, and the proper way to fix it is
// to integrate nodegit-lfs into nodegit. Until this is implemented, a
// temporary workaround has been applied, which affects only Workers leveraging
// threaded libgit2 functions (at the moment only checkout) and does the
// following:
// - do not wait for the current callback to end, so that it can send the
//  next callback to the main JS thread.
// - do not temporary unlock the objects, since they would be locked back
//  again before the callback is executed.

namespace nodegit {
  class Context;
  class AsyncContextCleanupHandle;
  class ThreadPoolImpl;

  class ThreadPool {
    public:
      typedef std::function<void()> Callback;
      typedef std::function<void(Callback, Callback)> QueueCallbackFn;
      // Temporary workaround for LFS checkout. Code modified to be reverted.
      // typedef std::function<Callback(QueueCallbackFn, Callback)> OnPostCallbackFn;
      typedef std::function<Callback(QueueCallbackFn, Callback, bool)> OnPostCallbackFn;

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

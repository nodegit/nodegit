#ifndef ASYNC_LIBGIT2_QUEUE_WORKER_H
#define ASYNC_LIBGIT2_QUEUE_WORKER_H

#include <nan.h>
#include <uv.h>
#include "nodegit.h"

// Frees the uv_async_t handle
NAN_INLINE void AsyncLibgit2Free(uv_handle_t* uv_async) {
  // freeing the handle can crash the app... perhaps libuv needs it
  // after the async callback returns, in which case we can't free it from
  // within the async callback...
  // free(uv_async);
}

// Runs WorkComplete of the scheduled AsyncWorker,
// and destroys it.  This is run in the uv_default_loop event loop.
NAN_INLINE void AsyncLibgit2Complete (uv_async_t* uv_async) {
  Nan::AsyncWorker *worker = static_cast<Nan::AsyncWorker*>(uv_async->data);
  worker->WorkComplete();
  worker->Destroy();
  // it doesn't look like this is a good place to close the handle
  // - it can cause the tests to hang - perhaps libuv expects it to be
  // upen throughout the callback
  // uv_close((uv_handle_t *)uv_async, AsyncLibgit2Free);
}

// Runs Execute of the scheduled AyncWorker on the dedicated libgit2 thread /
// event loop, and schedules the WorkComplete callback to run on the
// uv_default_loop event loop
NAN_INLINE void AsyncLibgit2Execute (uv_async_t* uv_async) {
  Nan::AsyncWorker *worker = static_cast<Nan::AsyncWorker*>(uv_async->data);
  worker->Execute();
  // it doesn't look like this is a good place to clean up the handle
  // - it can cause the tests to hang - perhaps libuv expects it to be
  // upen throughout the callback
  // uv_close((uv_handle_t *)uv_async, AsyncLibgit2Free);

  // maybe we can reuse the first handle?  not sure, making a new one
  uv_async = (uv_async_t *)malloc(sizeof(uv_async_t));
  uv_async_init(uv_default_loop(), uv_async, AsyncLibgit2Complete);
  uv_async->data = worker;
  uv_async_send(uv_async);
}

// Schedules the AsyncWorker to run on the dedicated libgit2 thread / event loop.
NAN_INLINE void AsyncLibgit2QueueWorker (Nan::AsyncWorker* worker) {
  uv_async_t *uv_async = (uv_async_t *)malloc(sizeof(uv_async_t));
  uv_async_init(libgit2_loop, uv_async, AsyncLibgit2Execute);
  uv_async->data = worker;
  uv_async_send(uv_async);
}

#endif

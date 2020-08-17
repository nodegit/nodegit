#include <nan.h>
#include "../include/thread_pool.h"

ThreadPool::ThreadPool(int numberOfThreads, uv_loop_t *loop)
{
  uv_async_init(loop, &loopAsync, RunLoopCallbacks);
  loopAsync.data = this;
  uv_unref((uv_handle_t *)&loopAsync);

  workInProgressCount = 0;

  for (int i=0; i<numberOfThreads; i++) {
    std::thread thread(&ThreadPool::RunEventQueue, this);
    threads.push_back(std::move(thread));
  }
}

void ThreadPool::QueueWork(Callback workCallback, Callback completionCallback, void *data) {
  {
    std::lock_guard<std::mutex> lock(workMutex);
    // there is work on the thread pool - reference the handle so
    // node doesn't terminate
    uv_ref((uv_handle_t *)&loopAsync);
    workQueue.push(Work(workCallback, completionCallback, data));
    workInProgressCount++;
  }
  workSemaphore.post();
}

void ThreadPool::QueueLoopCallback(Callback callback, void *data, bool isWork) {
  // push the callback into the queue
  std::lock_guard<std::mutex> lock(loopMutex);
  LoopCallback loopCallback(callback, data, isWork);
  bool queueWasEmpty = loopQueue.empty();
  loopQueue.push(loopCallback);
  // we only trigger RunLoopCallbacks via the loopAsync handle if the queue
  // was empty.  Otherwise, we depend on RunLoopCallbacks to re-trigger itself
  if (queueWasEmpty) {
    uv_async_send(&loopAsync);
  }
}

void ThreadPool::ExecuteReverseCallback(Callback reverseCallback, void *data) {
  QueueLoopCallback(reverseCallback, data, false);
}

void ThreadPool::RunEventQueue() {
  for ( ; ; ) {
    // wait until there is work to do
    workSemaphore.wait();
    Work work;
    {
      std::lock_guard<std::mutex> lock(workMutex);
      // the semaphore should guarantee that queue is not empty
      work = workQueue.front();
      workQueue.pop();
    }

    // perform the queued work
    (*work.workCallback)(work.data);

    // schedule the completion callback on the loop
    QueueLoopCallback(work.completionCallback, work.data, true);
  }
}

void ThreadPool::RunLoopCallbacks(uv_async_t* handle) {
  static_cast<ThreadPool *>(handle->data)->RunLoopCallbacks();
}

void ThreadPool::RunLoopCallbacks() {
  Nan::HandleScope scope;
  v8::Local<v8::Context> context = Nan::GetCurrentContext();
  node::CallbackScope callbackScope(context->GetIsolate(), Nan::New<v8::Object>(), {0, 0});
  LoopCallback loopCallback;
  {
    std::lock_guard<std::mutex> lock(loopMutex);
    // get the next callback to run
    loopCallback = loopQueue.front();
  }

  // perform the queued loop callback
  (*loopCallback.callback)(loopCallback.data);

  // pop the queue, and if necessary, re-trigger RunLoopCallbacks
  {
    std::lock_guard<std::mutex> lock(loopMutex);
    loopQueue.pop();
    if (!loopQueue.empty()) {
      uv_async_send(&loopAsync);
    }
  }

  // if there is no ongoing work / completion processing, node doesn't need
  // to be prevented from terminating
  if (loopCallback.isWork) {
    std::lock_guard<std::mutex> lock(workMutex);
    workInProgressCount --;
    if(!workInProgressCount) {
      uv_unref((uv_handle_t *)&loopAsync);
    }
  }
}

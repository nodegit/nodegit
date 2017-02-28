#include "../include/thread_pool.h"

ThreadPool::ThreadPool(int numberOfThreads, uv_loop_t *loop) {
  uv_mutex_init(&workMutex);
  uv_sem_init(&workSemaphore, 0);

  uv_async_init(loop, &loopAsync, RunLoopCallbacks);
  loopAsync.data = this;
  uv_unref((uv_handle_t *)&loopAsync);
  uv_mutex_init(&loopMutex);

  workInProgressCount = 0;

  for(int i=0; i<numberOfThreads; i++) {
    uv_thread_t thread;
    uv_thread_create(&thread, RunEventQueue, this);
  }
}

void ThreadPool::QueueWork(Callback workCallback, Callback completionCallback, void *data) {
  uv_mutex_lock(&workMutex);
  // there is work on the thread pool - reference the handle so
  // node doesn't terminate
  uv_ref((uv_handle_t *)&loopAsync);
  workQueue.push(Work(workCallback, completionCallback, data));
  workInProgressCount++;
  uv_mutex_unlock(&workMutex);
  uv_sem_post(&workSemaphore);
}

void ThreadPool::QueueLoopCallback(Callback callback, void *data, bool isWork) {
  // push the callback into the queue
  uv_mutex_lock(&loopMutex);
  LoopCallback loopCallback(callback, data, isWork);
  bool queueWasEmpty = loopQueue.empty();
  loopQueue.push(loopCallback);
  // we only trigger RunLoopCallbacks via the loopAsync handle if the queue
  // was empty.  Otherwise, we depend on RunLoopCallbacks to re-trigger itself
  if (queueWasEmpty) {
    uv_async_send(&loopAsync);
  }
  uv_mutex_unlock(&loopMutex);
}

void ThreadPool::ExecuteReverseCallback(Callback reverseCallback, void *data) {
  QueueLoopCallback(reverseCallback, data, false);
}

void ThreadPool::RunEventQueue(void *threadPool) {
  static_cast<ThreadPool *>(threadPool)->RunEventQueue();
}

void ThreadPool::RunEventQueue() {
  for ( ; ; ) {
    // wait until there is work to do
    uv_sem_wait(&workSemaphore);
    uv_mutex_lock(&workMutex);
    // the semaphore should guarantee that queue is not empty
    Work work = workQueue.front();
    workQueue.pop();
    uv_mutex_unlock(&workMutex);

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
  // get the next callback to run
  uv_mutex_lock(&loopMutex);
  LoopCallback loopCallback = loopQueue.front();
  uv_mutex_unlock(&loopMutex);

  // perform the queued loop callback
  (*loopCallback.callback)(loopCallback.data);

  // pop the queue, and if necessary, re-trigger RunLoopCallbacks
  uv_mutex_lock(&loopMutex);
  loopQueue.pop();
  if (!loopQueue.empty()) {
    uv_async_send(&loopAsync);
  }
  uv_mutex_unlock(&loopMutex);

  // if there is no ongoing work / completion processing, node doesn't need
  // to be prevented from terminating
  if (loopCallback.isWork) {
    uv_mutex_lock(&workMutex);
    workInProgressCount --;
    if(!workInProgressCount) {
      uv_unref((uv_handle_t *)&loopAsync);
    }
    uv_mutex_unlock(&workMutex);
  }
}

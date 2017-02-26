#include "../include/thread_pool.h"

ThreadPool::ThreadPool(int numberOfThreads, uv_loop_t *loop) {
  uv_mutex_init(&workMutex);
  uv_sem_init(&workSemaphore, 0);

  uv_async_init(loop, &completionAsync, RunCompletionCallbacks);
  completionAsync.data = this;
  uv_unref((uv_handle_t *)&completionAsync);
  uv_mutex_init(&completionMutex);

  uv_async_init(loop, &reverseAsync, RunReverseCallbacks);
  reverseAsync.data = this;
  uv_unref((uv_handle_t *)&reverseAsync);
  uv_mutex_init(&reverseMutex);

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
  uv_ref((uv_handle_t *)&completionAsync);
  workQueue.push(Work(workCallback, completionCallback, data));
  workInProgressCount++;
  uv_mutex_unlock(&workMutex);
  uv_sem_post(&workSemaphore);
}

void ThreadPool::ExecuteReverseCallback(Callback reverseCallback, void *data) {
  // push the callback into the queue
  uv_mutex_lock(&reverseMutex);
  ReverseCall reverseCall(reverseCallback, data);
  bool queueWasEmpty = reverseQueue.empty();
  reverseQueue.push(reverseCall);
  // we only trigger RunReverseCallbacks via the reverseAsync handle if the queue
  // was empty.  Otherwise, we depend on RunReverseCallbacks to re-trigger itself
  if (queueWasEmpty) {
    uv_async_send(&reverseAsync);
  }
  uv_mutex_unlock(&reverseMutex);
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

    // schedule the callback on the loop
    uv_mutex_lock(&completionMutex);
    completionQueue.push(work);
    uv_mutex_unlock(&completionMutex);
    uv_async_send(&completionAsync);
  }
}

void ThreadPool::RunCompletionCallbacks(uv_async_t* handle) {
  static_cast<ThreadPool *>(handle->data)->RunCompletionCallbacks();
}

void ThreadPool::RunCompletionCallbacks() {
  // uv_async_send can coalesce calls, so we are not guaranteed one
  // RunCompletionCallbacks per uv_async_send call
  // so we always process the entire completionQueue
  int callbacksCompleted = 0;
  uv_mutex_lock(&completionMutex);
  while(!completionQueue.empty()) {
    Work work = completionQueue.front();
    completionQueue.pop();
    uv_mutex_unlock(&completionMutex);
    // perform the queued loop callback
    (*work.completionCallback)(work.data);
    callbacksCompleted++;
    uv_mutex_lock(&completionMutex);
  }
  uv_mutex_unlock(&completionMutex);

  uv_mutex_lock(&workMutex);
  // if there is no ongoing work / completion processing, node doesn't need
  // to be prevented from terminating
  workInProgressCount -= callbacksCompleted;
  if(!workInProgressCount) {
    uv_unref((uv_handle_t *)&completionAsync);
  }
  uv_mutex_unlock(&workMutex);
}

void ThreadPool::RunReverseCallbacks(uv_async_t* handle) {
  static_cast<ThreadPool *>(handle->data)->RunReverseCallbacks();
}

void ThreadPool::RunReverseCallbacks() {
  // get the next callback to run
  uv_mutex_lock(&reverseMutex);
  ReverseCall reverseCall = reverseQueue.front();
  uv_mutex_unlock(&reverseMutex);

  // execute callback
  (*reverseCall.reverseCallback)(reverseCall.data);

  // pop the queue, and if necessary, re-trigger RunReverseCallbacks
  uv_mutex_lock(&reverseMutex);
  reverseQueue.pop();
  if (!reverseQueue.empty()) {
    uv_async_send(&reverseAsync);
  }
  uv_mutex_unlock(&reverseMutex);
}

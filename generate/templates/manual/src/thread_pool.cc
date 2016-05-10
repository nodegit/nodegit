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

void ThreadPool::QueueWork(Callback workCallback, Callback loopCallback, void *data) {
  uv_mutex_lock(&workMutex);
  // there is work on the thread pool - reference the handle so
  // node doesn't terminate
  uv_ref((uv_handle_t *)&loopAsync);
  workQueue.push(Work(workCallback, loopCallback, data));
  workInProgressCount++;
  uv_mutex_unlock(&workMutex);
  uv_sem_post(&workSemaphore);
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
    uv_mutex_lock(&loopMutex);
    loopQueue.push(work);
    uv_mutex_unlock(&loopMutex);
    uv_async_send(&loopAsync);
  }
}

void ThreadPool::RunLoopCallbacks(uv_async_t* handle) {
  static_cast<ThreadPool *>(handle->data)->RunLoopCallbacks();
}

void ThreadPool::RunLoopCallbacks() {
  // uv_async_send can coalesce calls, so we are not guaranteed one
  // RunLoopCallbacks per uv_async_send call
  // so we always process the entire loopQueue
  int callbacksCompleted = 0;
  uv_mutex_lock(&loopMutex);
  while(!loopQueue.empty()) {
    Work work = loopQueue.front();
    loopQueue.pop();
    uv_mutex_unlock(&loopMutex);
    // perform the queued loop callback
    (*work.loopCallback)(work.data);
    callbacksCompleted++;
    uv_mutex_lock(&loopMutex);
  }
  uv_mutex_unlock(&loopMutex);

  uv_mutex_lock(&workMutex);
  // if there is no ongoing work / completion processing, node doesn't need
  // to be prevented from terminating
  workInProgressCount -= callbacksCompleted;
  if(!workInProgressCount) {
    uv_unref((uv_handle_t *)&loopAsync);
  }
  uv_mutex_unlock(&workMutex);
}

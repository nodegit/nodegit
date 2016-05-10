#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <uv.h>
#include <queue>

class ThreadPool {
  typedef void (*Callback) (void *);
  struct Work {
    Callback workCallback;
    Callback loopCallback;
    void *data;

    Work(Callback workCallback, Callback loopCallback, void *data)
      : workCallback(workCallback), loopCallback(loopCallback), data(data) {
    }
  };

  // work to be performed on the threadpool
  std::queue<Work> workQueue;
  uv_mutex_t workMutex;
  uv_sem_t workSemaphore;
  int workInProgressCount;

  // completion callbacks to be performed on the loop
  std::queue<Work> loopQueue;
  uv_mutex_t loopMutex;
  uv_async_t loopAsync;

  static void RunEventQueue(void *threadPool);
  void RunEventQueue();
  static void RunLoopCallbacks(uv_async_t* handle);
  void RunLoopCallbacks();
public:
  // Initializes thread pool and spins up the requested number of threads
  // The provided loop will be used for completion callbacks, whenever
  // queued work is completed
  ThreadPool(int numberOfThreads, uv_loop_t *loop);
  // Queues work on the thread pool, followed by completion call scheduled
  // on the loop provided in the constructor.
  // QueueWork should be called on the loop provided in the constructor.
  void QueueWork(Callback workCallback, Callback loopCallback, void *data);
};

#endif

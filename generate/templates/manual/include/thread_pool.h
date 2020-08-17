#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <uv.h>
#include <mutex>
#include <queue>
#include <thread>

#include "./semaphore.h"

class ThreadPool {
public:
  typedef void (*Callback) (void *);

private:
  struct Work {
    Callback workCallback;
    Callback completionCallback;
    void *data;

    Work(Callback workCallback, Callback completionCallback, void *data)
      : workCallback(workCallback), completionCallback(completionCallback), data(data)
    {}

    Work()
      : workCallback(NULL), completionCallback(NULL), data(NULL)
    {}
  };

  struct LoopCallback {
    Callback callback;
    void *data;
    bool isWork;

    LoopCallback(Callback callback, void *data, bool isWork)
      : callback(callback), data(data), isWork(isWork)
    {}

    LoopCallback()
      : callback(NULL), data(NULL), isWork(false)
    {}
  };

  // work to be performed on the threadpool
  std::queue<Work> workQueue;
  std::mutex workMutex;
  Semaphore workSemaphore;
  int workInProgressCount;

  // completion and async callbacks to be performed on the loop
  std::queue<LoopCallback> loopQueue;
  std::mutex loopMutex;
  uv_async_t loopAsync;

  std::vector<std::thread> threads;

  void RunEventQueue();
  static void RunLoopCallbacks(uv_async_t* handle);
  void RunLoopCallbacks();

  void QueueLoopCallback(Callback callback, void *data, bool isWork);

public:
  // Initializes thread pool and spins up the requested number of threads
  // The provided loop will be used for completion callbacks, whenever
  // queued work is completed
  ThreadPool(int numberOfThreads, uv_loop_t *loop);
  // Queues work on the thread pool, followed by completion call scheduled
  // on the loop provided in the constructor.
  // QueueWork should be called on the loop provided in the constructor.
  void QueueWork(Callback workCallback, Callback completionCallback, void *data);
  // Queues a callback on the loop provided in the constructor
  void ExecuteReverseCallback(Callback reverseCallback, void *data);
};

#endif

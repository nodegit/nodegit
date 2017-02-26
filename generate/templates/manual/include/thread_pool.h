#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <uv.h>
#include <queue>

class ThreadPool {
public:
  typedef void (*Callback) (void *);

private:
  struct Work {
    Callback workCallback;
    Callback completionCallback;
    void *data;

    Work(Callback workCallback, Callback completionCallback, void *data)
      : workCallback(workCallback), completionCallback(completionCallback), data(data) {
    }
  };

  struct ReverseCall {
    Callback reverseCallback;
    void *data;

    ReverseCall(Callback reverseCallback, void *data)
      : reverseCallback(reverseCallback), data(data) {
    }
  };

  // work to be performed on the threadpool
  std::queue<Work> workQueue;
  uv_mutex_t workMutex;
  uv_sem_t workSemaphore;
  int workInProgressCount;

  // completion callbacks to be performed on the loop
  std::queue<Work> completionQueue;
  uv_mutex_t completionMutex;
  uv_async_t completionAsync;

  // async callback made from the threadpool, executed in the loop
  std::queue<ReverseCall> reverseQueue;
  uv_mutex_t reverseMutex;
  uv_async_t reverseAsync;

  static void RunEventQueue(void *threadPool);
  void RunEventQueue();
  static void RunCompletionCallbacks(uv_async_t* handle);
  void RunCompletionCallbacks();
  static void RunReverseCallbacks(uv_async_t *handle);
  void RunReverseCallbacks();

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
  // these block the calling thread's execution until the callback completes
  void ExecuteReverseCallback(Callback reverseCallback, void *data);
};

#endif

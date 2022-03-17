#include <nan.h>
#include "../include/context.h"
#include "../include/thread_pool.h"

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <atomic> // Temporary workaround for LFS checkout. Code added to be reverted.

extern "C" {
  #include <git2/sys/custom_tls.h>
}

using namespace std::placeholders;

namespace nodegit {
  class Executor {
    public:
      struct Task {
        enum Type { SHUTDOWN, WORK };

        Task(Type initType)
          : type(initType)
        {}
        Task(const Task &) = delete;
        Task(Task &&) = delete;
        Task &operator=(const Task &) = delete;
        Task &operator=(Task &&) = delete;

        // We must define a virtual destructor so that derived classes are castable
        virtual ~Task() {}

        Type type;
      };

      struct ShutdownTask : Task {
        ShutdownTask()
          : Task(SHUTDOWN)
        {}
      };

      struct WorkTask : Task {
        WorkTask(ThreadPool::Callback initCallback, Nan::AsyncResource *asyncResource, Nan::Global<v8::Value> *callbackErrorHandle)
          : Task(WORK), asyncResource(asyncResource), callbackErrorHandle(callbackErrorHandle), callback(initCallback)
        {}

        Nan::AsyncResource *asyncResource;
        Nan::Global<v8::Value> *callbackErrorHandle;
        ThreadPool::Callback callback;
      };

      typedef std::function<void(ThreadPool::OnPostCallbackFn)> PostCallbackEventToOrchestratorFn;
      typedef std::function<void()> PostCompletedEventToOrchestratorFn;
      typedef std::function<std::unique_ptr<Task>()> TakeNextTaskFn;

      struct Event {
        enum Type { COMPLETED, CALLBACK_TYPE };
        Event(Type initType)
          : type(initType)
        {}
        Event(const Event &) = delete;
        Event(Event &&) = delete;
        Event &operator=(const Event &) = delete;
        Event &operator=(Event &&) = delete;

        Type type;

        // We must define a virtual destructor so that derived classes are castable
        virtual ~Event() {}
      };

      struct CompletedEvent : Event {
        CompletedEvent()
          : Event(COMPLETED)
        {}
      };

      struct CallbackEvent : Event {
        CallbackEvent(ThreadPool::OnPostCallbackFn initCallback)
          : Event(CALLBACK_TYPE), callback(initCallback)
        {}

        // Temporary workaround for LFS checkout. Code modified to be reverted.
        // ThreadPool::Callback operator()(ThreadPool::QueueCallbackFn queueCb, ThreadPool::Callback completedCb) {
        //   return callback(queueCb, completedCb);
        ThreadPool::Callback operator()(ThreadPool::QueueCallbackFn queueCb, ThreadPool::Callback completedCb, bool isThreaded) {
          return callback(queueCb, completedCb, isThreaded);
        }

        private:
          ThreadPool::OnPostCallbackFn callback;
      };

      Executor(
        PostCallbackEventToOrchestratorFn postCallbackEventToOrchestrator,
        PostCompletedEventToOrchestratorFn postCompletedEventToOrchestrator,
        TakeNextTaskFn takeNextTask,
        nodegit::Context *context
      );

      void RunTaskLoop();

      // Orchestrator needs to call this to ensure that the executor is done reading from
      // the Orchestrator's memory
      void WaitForThreadClose();

      // Temporary workaround for LFS checkout. Code added to be reverted.
      // Returns true if the task running spawned threads within libgit2
      bool IsGitThreaded() { return currentGitThreads > kInitialGitThreads; }

      static Nan::AsyncResource *GetCurrentAsyncResource();

      static const nodegit::Context *GetCurrentContext();

      static Nan::Global<v8::Value> *GetCurrentCallbackErrorHandle();

      static void PostCallbackEvent(ThreadPool::OnPostCallbackFn onPostCallback);

      // Libgit2 will call this before it spawns a child thread.
      // That way we can decide what the TLS for that thread should be
      // We will make sure that the context for the current async work
      // is preserved on the child thread through this method
      static void *RetrieveTLSForLibgit2ChildThread();

      // Libgit2 will call this on a child thread with the pointer that was
      // retrieved from RetrieveTLSForLibgit2ChildThread. That allows us
      // to store the necessary thread local storage for the child thread
      static void SetTLSForLibgit2ChildThread(void *vexecutor);

      // Called when a libgit2 child thread exits. This gives us the ability
      // to teardown any TLS we set up for the child thread if we need to
      static void TeardownTLSOnLibgit2ChildThread();

    private:
      Nan::AsyncResource *currentAsyncResource;
      Nan::Global<v8::Value> *currentCallbackErrorHandle;
      nodegit::Context *currentContext;

      // We need to populate the executor on every thread that libgit2
      // could make a callback on so that it can correctly queue callbacks
      // in the correct javascript context
      thread_local static Executor *executor;
      thread_local static bool isExecutorThread;
      PostCallbackEventToOrchestratorFn postCallbackEventToOrchestrator;
      PostCompletedEventToOrchestratorFn postCompletedEventToOrchestrator;
      TakeNextTaskFn takeNextTask;
      std::thread thread;

      // Temporary workaround for LFS checkout. Code added to be reverted.
      static constexpr int kInitialGitThreads {0};
      // Number of threads spawned internally by libgit2 to deal with
      // the task of this Executor instance. Defaults to kInitialGitThreads.
      std::atomic<int> currentGitThreads {kInitialGitThreads};      
  };

  Executor::Executor(
    PostCallbackEventToOrchestratorFn postCallbackEventToOrchestrator,
    PostCompletedEventToOrchestratorFn postCompletedEventToOrchestrator,
    TakeNextTaskFn takeNextTask,
    nodegit::Context *context
  )
    : currentAsyncResource(nullptr),
      currentCallbackErrorHandle(nullptr),
      currentContext(context),
      postCallbackEventToOrchestrator(postCallbackEventToOrchestrator),
      postCompletedEventToOrchestrator(postCompletedEventToOrchestrator),
      takeNextTask(takeNextTask),
      thread(&Executor::RunTaskLoop, this)
  {}

  void Executor::RunTaskLoop() {
    // Set the thread local storage so that libgit2 can pick up the current executor
    // for the thread.
    isExecutorThread = true;
    executor = this;

    for ( ; ; ) {
      std::unique_ptr<Task> task = takeNextTask();
      if (task->type == Task::Type::SHUTDOWN) {
        return;
      }

      WorkTask *workTask = static_cast<WorkTask *>(task.get());

      // Temporary workaround for LFS checkout. Code added to be reverted.
      currentGitThreads = kInitialGitThreads;

      currentAsyncResource = workTask->asyncResource;
      currentCallbackErrorHandle = workTask->callbackErrorHandle;
      workTask->callback();
      currentCallbackErrorHandle = nullptr;
      currentAsyncResource = nullptr;

      postCompletedEventToOrchestrator();
    }
  }

  void Executor::WaitForThreadClose() {
    thread.join();
  }

  Nan::AsyncResource *Executor::GetCurrentAsyncResource() {
    if (executor) {
      return executor->currentAsyncResource;
    }

    // NOTE this should always be set when a libgit2 callback is running,
    //      so this case should not happen.
    return nullptr;
  }

  const nodegit::Context *Executor::GetCurrentContext() {
    if (executor) {
      return executor->currentContext;
    }

    // NOTE this should always be set when a libgit2 callback is running,
    //      so this case should not happen.
    return nullptr;
  }

  Nan::Global<v8::Value> *Executor::GetCurrentCallbackErrorHandle() {
    if (executor) {
      return executor->currentCallbackErrorHandle;
    }

    // NOTE this should always be set when a libgit2 callback is running,
    //      so this case should not happen.
    return nullptr;
  }

  void Executor::PostCallbackEvent(ThreadPool::OnPostCallbackFn onPostCallback) {
    if (executor) {
      executor->postCallbackEventToOrchestrator(onPostCallback);
    }
  }

  void *Executor::RetrieveTLSForLibgit2ChildThread() {
    // Temporary workaround for LFS checkout. Code added to be reverted.
    ++Executor::executor->currentGitThreads;
    return Executor::executor;
  }

  void Executor::SetTLSForLibgit2ChildThread(void *vexecutor) {
    Executor::executor = static_cast<Executor *>(vexecutor);
  }

  void Executor::TeardownTLSOnLibgit2ChildThread() {
    if (!isExecutorThread) {
      // Temporary workaround for LFS checkout. Code added to be reverted.
      --Executor::executor->currentGitThreads;
      Executor::executor = nullptr;
    }
  }

  thread_local Executor *Executor::executor = nullptr;
  thread_local bool Executor::isExecutorThread = false;

  class Orchestrator {
    public:
      struct Job {
        enum Type { SHUTDOWN, ASYNC_WORK };
        Job(Type initType)
          : type(initType)
        {}
        Job(const Job &) = delete;
        Job(Job &&) = delete;
        Job &operator=(const Job &) = delete;
        Job &operator=(Job &&) = delete;

        virtual ~Job() {}

        Type type;
      };

      struct ShutdownJob : Job {
        ShutdownJob()
          : Job(SHUTDOWN)
        {}
      };

      struct AsyncWorkJob : Job {
        AsyncWorkJob(nodegit::AsyncWorker *initWorker)
          : Job(ASYNC_WORK), worker(initWorker)
        {}

        nodegit::AsyncWorker *worker;
      };

      typedef std::function<void(ThreadPool::Callback, ThreadPool::Callback, bool)> QueueCallbackOnJSThreadFn;
      typedef std::function<std::shared_ptr<Job>()> TakeNextJobFn;

    private:
      class OrchestratorImpl {
        public:
          OrchestratorImpl(
            QueueCallbackOnJSThreadFn queueCallbackOnJSThread,
            TakeNextJobFn takeNextJob,
            nodegit::Context *context
          );

          void RunJobLoop();

          // The Executor will call this method to queue a CallbackEvent in Orchestrator's event loop
          void PostCallbackEvent(ThreadPool::OnPostCallbackFn onPostCallback);

          // The Executor will call this method after completion its work. This should queue
          // a CompletedEvent in Thread's event loop
          void PostCompletedEvent();

          // This will be used by Executor to take jobs that the Thread has picked up and run them.
          std::unique_ptr<Executor::Task> TakeNextTask();

          // This is used to wait for the Orchestrator's thread to shutdown after signaling shutdown
          void WaitForThreadClose();

        private:
          // The only thread safe way to pull events from executorEventsQueue
          std::shared_ptr<Executor::Event> TakeEventFromExecutor();

          void ScheduleWorkTaskOnExecutor(ThreadPool::Callback callback, Nan::AsyncResource *asyncResource, Nan::Global<v8::Value> *callbackErrorHandle);

          void ScheduleShutdownTaskOnExecutor();

          std::condition_variable taskCondition;
          std::unique_ptr<std::mutex> taskMutex;

          std::queue<std::shared_ptr<Executor::Event>> executorEventsQueue;
          std::unique_ptr<std::mutex> executorEventsMutex;
          std::condition_variable executorEventsCondition;

          QueueCallbackOnJSThreadFn queueCallbackOnJSThread;
          TakeNextJobFn takeNextJob;
          std::unique_ptr<Executor::Task> task;
          std::thread thread;
          Executor executor;
      };

      std::unique_ptr<OrchestratorImpl> impl;

    public:
      Orchestrator(
        QueueCallbackOnJSThreadFn queueCallbackOnJSThread,
        TakeNextJobFn takeNextJob,
        nodegit::Context *context
      );

      void WaitForThreadClose();
  };

  Orchestrator::OrchestratorImpl::OrchestratorImpl(
    QueueCallbackOnJSThreadFn queueCallbackOnJSThread,
    TakeNextJobFn takeNextJob,
    nodegit::Context *context
  )
    : taskMutex(new std::mutex),
      executorEventsMutex(new std::mutex),
      queueCallbackOnJSThread(queueCallbackOnJSThread),
      takeNextJob(takeNextJob),
      task(nullptr),
      thread(&Orchestrator::OrchestratorImpl::RunJobLoop, this),
      executor(
        std::bind(&Orchestrator::OrchestratorImpl::PostCallbackEvent, this, _1),
        std::bind(&Orchestrator::OrchestratorImpl::PostCompletedEvent, this),
        std::bind(&Orchestrator::OrchestratorImpl::TakeNextTask, this),
        context
      )
  {}

  void Orchestrator::OrchestratorImpl::RunJobLoop() {
    for ( ; ; ) {
      auto job = takeNextJob();
      switch (job->type) {
        case Job::Type::SHUTDOWN: {
          ScheduleShutdownTaskOnExecutor();
          executor.WaitForThreadClose();
          return;
        }

        case Job::Type::ASYNC_WORK: {
          std::shared_ptr<AsyncWorkJob> asyncWorkJob = std::static_pointer_cast<AsyncWorkJob>(job);
          nodegit::AsyncWorker *worker = asyncWorkJob->worker;
          // We lock at this level, because we temporarily unlock the lock master
          // when a callback is fired. We need to be on the same thread to ensure
          // the same thread that acquired the locks also releases them
          nodegit::LockMaster lock = worker->AcquireLocks();
          ScheduleWorkTaskOnExecutor(std::bind(&nodegit::AsyncWorker::Execute, worker), worker->GetAsyncResource(), worker->GetCallbackErrorHandle());
          for ( ; ; ) {
            std::shared_ptr<Executor::Event> event = TakeEventFromExecutor();
            if (event->type == Executor::Event::Type::COMPLETED) {
              break;
            }

            // We must have received a callback from libgit2
            auto callbackEvent = std::static_pointer_cast<Executor::CallbackEvent>(event);
            std::shared_ptr<std::mutex> callbackMutex(new std::mutex);
            std::shared_ptr<std::condition_variable> callbackCondition(new std::condition_variable);
            bool hasCompleted = false;

            // Temporary workaround for LFS checkout. Code removed to be reverted.
            //LockMaster::TemporaryUnlock temporaryUnlock;

            // Temporary workaround for LFS checkout. Code added to be reverted.
            bool isWorkerThreaded = executor.IsGitThreaded();
            ThreadPool::Callback callbackCompleted = []() {};
            if (!isWorkerThreaded) {
              callbackCompleted = [callbackCondition, callbackMutex, &hasCompleted]() {
                std::lock_guard<std::mutex> lock(*callbackMutex);
                hasCompleted = true;
                callbackCondition->notify_one();
              };
            }
            std::unique_ptr<LockMaster::TemporaryUnlock> temporaryUnlock {nullptr};
            if (!isWorkerThreaded) {
              temporaryUnlock = std::make_unique<LockMaster::TemporaryUnlock>();
            }

            auto onCompletedCallback = (*callbackEvent)(
              [this](ThreadPool::Callback callback, ThreadPool::Callback cancelCallback) {
                queueCallbackOnJSThread(callback, cancelCallback, false);
              },
              // Temporary workaround for LFS checkout. Code modified to be reverted.
              /*
              [callbackCondition, callbackMutex, &hasCompleted]() {
                std::lock_guard<std::mutex> lock(*callbackMutex);
                hasCompleted = true;
                callbackCondition->notify_one();
              }
              */
              callbackCompleted,
              isWorkerThreaded
            );

            // Temporary workaround for LFS checkout. Code modified to be reverted.
            if (!isWorkerThreaded) {
              std::unique_lock<std::mutex> lock(*callbackMutex);
              while (!hasCompleted) callbackCondition->wait(lock);
              onCompletedCallback();
            }
          }

          queueCallbackOnJSThread(
            [worker]() {
              worker->WorkComplete();
              worker->Destroy();
            },
            [worker]() {
              worker->Cancel();
              worker->WorkComplete();
              worker->Destroy();
            },
            true
          );
        }
      }
    }
  }

  // TODO add a cancel callback to `OnPostCallbackFn` which can be used on nodegit terminate
  void Orchestrator::OrchestratorImpl::PostCallbackEvent(ThreadPool::OnPostCallbackFn onPostCallback) {
    std::lock_guard<std::mutex> lock(*executorEventsMutex);
    std::shared_ptr<Executor::CallbackEvent> callbackEvent(new Executor::CallbackEvent(onPostCallback));
    executorEventsQueue.push(callbackEvent);
    executorEventsCondition.notify_one();
  }

  void Orchestrator::OrchestratorImpl::PostCompletedEvent() {
    std::lock_guard<std::mutex> lock(*executorEventsMutex);
    std::shared_ptr<Executor::CompletedEvent> completedEvent(new Executor::CompletedEvent);
    executorEventsQueue.push(completedEvent);
    executorEventsCondition.notify_one();
  }

  std::shared_ptr<Executor::Event> Orchestrator::OrchestratorImpl::TakeEventFromExecutor() {
    std::unique_lock<std::mutex> lock(*executorEventsMutex);
    while (executorEventsQueue.empty()) executorEventsCondition.wait(lock);
    std::shared_ptr<Executor::Event> executorEvent = executorEventsQueue.front();
    executorEventsQueue.pop();
    return executorEvent;
  }

  void Orchestrator::OrchestratorImpl::ScheduleShutdownTaskOnExecutor() {
    std::lock_guard<std::mutex> lock(*taskMutex);
    task.reset(new Executor::ShutdownTask);
    taskCondition.notify_one();
  }

  void Orchestrator::OrchestratorImpl::ScheduleWorkTaskOnExecutor(ThreadPool::Callback callback, Nan::AsyncResource *asyncResource, Nan::Global<v8::Value> *callbackErrorHandle) {
    std::lock_guard<std::mutex> lock(*taskMutex);
    task.reset(new Executor::WorkTask(callback, asyncResource, callbackErrorHandle));
    taskCondition.notify_one();
  }

  std::unique_ptr<Executor::Task> Orchestrator::OrchestratorImpl::TakeNextTask() {
    std::unique_lock<std::mutex> lock(*taskMutex);
    while (!task) taskCondition.wait(lock);
    return std::move(task);
  }

  void Orchestrator::OrchestratorImpl::WaitForThreadClose() {
    thread.join();
  }

  Orchestrator::Orchestrator(
    QueueCallbackOnJSThreadFn queueCallbackOnJSThread,
    TakeNextJobFn takeNextJob,
    nodegit::Context *context
  )
    : impl(new OrchestratorImpl(queueCallbackOnJSThread, takeNextJob, context))
  {}

  void Orchestrator::WaitForThreadClose() {
    impl->WaitForThreadClose();
  }

  class ThreadPoolImpl {
    public:
      ThreadPoolImpl(int numberOfThreads, uv_loop_t *loop, nodegit::Context *context);

      void QueueWorker(nodegit::AsyncWorker *worker);

      std::shared_ptr<Orchestrator::Job> TakeNextJob();

      void QueueCallbackOnJSThread(ThreadPool::Callback callback, ThreadPool::Callback cancelCallback, bool isWork);

      static void RunLoopCallbacks(uv_async_t *handle);

      void Shutdown(std::unique_ptr<AsyncContextCleanupHandle> cleanupHandle);

      struct AsyncCallbackData {
        AsyncCallbackData(ThreadPoolImpl *pool)
          : pool(pool)
        {}

        std::unique_ptr<AsyncContextCleanupHandle> cleanupHandle;
        ThreadPoolImpl *pool;
      };

    private:
      bool isMarkedForDeletion;

      struct JSThreadCallback {
        JSThreadCallback(ThreadPool::Callback callback, ThreadPool::Callback cancelCallback, bool isWork)
          :  isWork(isWork), callback(callback), cancelCallback(cancelCallback)
        {}

        JSThreadCallback()
          : isWork(false), callback(nullptr), cancelCallback(nullptr)
        {}

        void performCallback() {
          callback();
        }

        void cancel() {
          cancelCallback();
        }

        bool isWork;

        private:
          ThreadPool::Callback callback;
          ThreadPool::Callback cancelCallback;
      };

      void RunLoopCallbacks();

      std::queue<std::shared_ptr<Orchestrator::Job>> orchestratorJobQueue;
      std::unique_ptr<std::mutex> orchestratorJobMutex;
      std::condition_variable orchestratorJobCondition;
      size_t workInProgressCount;

      // completion and async callbacks to be performed on the loop
      std::queue<JSThreadCallback> jsThreadCallbackQueue;
      std::unique_ptr<std::mutex> jsThreadCallbackMutex;
      uv_async_t jsThreadCallbackAsync;

      std::vector<Orchestrator> orchestrators;
  };

  // context required to be passed to Orchestrators, but ThreadPoolImpl doesn't need to keep it
  ThreadPoolImpl::ThreadPoolImpl(int numberOfThreads, uv_loop_t *loop, nodegit::Context *context)
    : isMarkedForDeletion(false),
      orchestratorJobMutex(new std::mutex),
      jsThreadCallbackMutex(new std::mutex)
  {
    uv_async_init(loop, &jsThreadCallbackAsync, RunLoopCallbacks);
    jsThreadCallbackAsync.data = new AsyncCallbackData(this);
    uv_unref((uv_handle_t *)&jsThreadCallbackAsync);

    workInProgressCount = 0;

    for (int i = 0; i < numberOfThreads; i++) {
      orchestrators.emplace_back(
        std::bind(&ThreadPoolImpl::QueueCallbackOnJSThread, this, _1, _2, _3),
        std::bind(&ThreadPoolImpl::TakeNextJob, this),
        context
      );
    }
  }

  void ThreadPoolImpl::QueueWorker(nodegit::AsyncWorker *worker) {
    std::lock_guard<std::mutex> lock(*orchestratorJobMutex);
    // there is work on the thread pool - reference the handle so
    // node doesn't terminate
    uv_ref((uv_handle_t *)&jsThreadCallbackAsync);
    orchestratorJobQueue.emplace(new Orchestrator::AsyncWorkJob(worker));
    workInProgressCount++;
    orchestratorJobCondition.notify_one();
  }

  std::shared_ptr<Orchestrator::Job> ThreadPoolImpl::TakeNextJob() {
    std::unique_lock<std::mutex> lock(*orchestratorJobMutex);
    while (orchestratorJobQueue.empty()) orchestratorJobCondition.wait(lock);
    auto orchestratorJob = orchestratorJobQueue.front();

    // When the thread pool is shutting down, the thread pool will drain the work queue and replace it with
    // a single shared_ptr to a shutdown job, so don't pop the queue when we're shutting down so
    // everyone gets the signal
    if (orchestratorJob->type != Orchestrator::Job::Type::SHUTDOWN) {
      orchestratorJobQueue.pop();
    }

    return orchestratorJob;
  }

  void ThreadPoolImpl::QueueCallbackOnJSThread(ThreadPool::Callback callback, ThreadPool::Callback cancelCallback, bool isWork) {
    std::unique_lock<std::mutex> lock(*jsThreadCallbackMutex);
    // When the threadpool is shutting down, we want to free up the executors to also shutdown
    // that means that we need to cancel all non-work callbacks as soon as we see them and
    // we know that we are shutting down
    if (isMarkedForDeletion && !isWork) {
      // we don't know how long the cancelCallback will take, and it certainly doesn't need the lock
      // while we're running it, so unlock it immediately.
      lock.unlock();
      cancelCallback();
      return;
    }

    bool queueWasEmpty = jsThreadCallbackQueue.empty();
    jsThreadCallbackQueue.emplace(callback, cancelCallback, isWork);
    // we only trigger RunLoopCallbacks via the jsThreadCallbackAsync handle if the queue
    // was empty.  Otherwise, we depend on RunLoopCallbacks to re-trigger itself
    if (queueWasEmpty) {
      uv_async_send(&jsThreadCallbackAsync);
    }
  }

  void ThreadPoolImpl::RunLoopCallbacks(uv_async_t* handle) {
    auto asyncCallbackData = static_cast<AsyncCallbackData *>(handle->data);
    if (asyncCallbackData->pool) {
      asyncCallbackData->pool->RunLoopCallbacks();
    }
  }

  // NOTE this should theoretically never be triggered during a cleanup operation
  void ThreadPoolImpl::RunLoopCallbacks() {
    Nan::HandleScope scope;
    v8::Local<v8::Context> context = Nan::GetCurrentContext();
    node::CallbackScope callbackScope(context->GetIsolate(), Nan::New<v8::Object>(), {0, 0});

    std::unique_lock<std::mutex> lock(*jsThreadCallbackMutex);
    // get the next callback to run
    JSThreadCallback jsThreadCallback = jsThreadCallbackQueue.front();
    jsThreadCallbackQueue.pop();

    lock.unlock();
    jsThreadCallback.performCallback();
    lock.lock();

    if (!jsThreadCallbackQueue.empty()) {
      uv_async_send(&jsThreadCallbackAsync);
    }

    // if there is no ongoing work / completion processing, node doesn't need
    // to be prevented from terminating
    if (jsThreadCallback.isWork) {
      std::lock_guard<std::mutex> orchestratorLock(*orchestratorJobMutex);
      workInProgressCount--;
      if (!workInProgressCount) {
        uv_unref((uv_handle_t *)&jsThreadCallbackAsync);
      }
    }
  }

  void ThreadPoolImpl::Shutdown(std::unique_ptr<AsyncContextCleanupHandle> cleanupHandle) {
    std::queue<std::shared_ptr<Orchestrator::Job>> cancelledJobs;
    std::queue<JSThreadCallback> cancelledCallbacks;
    {
      std::unique_lock<std::mutex> orchestratorLock(*orchestratorJobMutex, std::defer_lock);
      std::unique_lock<std::mutex> jsThreadLock(*jsThreadCallbackMutex, std::defer_lock);
      std::lock(orchestratorLock, jsThreadLock);

      // Once we've marked for deletion, we will start cancelling all callbacks
      // when an attempt to queue a callback is made
      isMarkedForDeletion = true;
      // We want to grab all of the jobs that have been queued and run their cancel routines
      // so that we can clean up their resources
      orchestratorJobQueue.swap(cancelledJobs);
      // We also want to grab all callbacks that have been queued so that we can
      // run their cancel routines, this will help terminate the async workers
      // that are currently being executed complete so that the threads
      // running them can exit cleanly
      jsThreadCallbackQueue.swap(cancelledCallbacks);
      // Pushing a ShutdownJob into the queue will instruct all threads
      // to start their shutdown process when they see the job is available.
      orchestratorJobQueue.emplace(new Orchestrator::ShutdownJob);

      if (workInProgressCount) {
        // unref the jsThreadCallback for all work in progress
        // it will not be used after this function has completed
        while (workInProgressCount--) {
          uv_unref((uv_handle_t *)&jsThreadCallbackAsync);
        }
      }

      orchestratorJobCondition.notify_all();
    }

    Nan::HandleScope scope;
    v8::Local<v8::Context> context = Nan::GetCurrentContext();
    node::CallbackScope callbackScope(context->GetIsolate(), Nan::New<v8::Object>(), {0, 0});

    while (cancelledJobs.size()) {
      std::shared_ptr<Orchestrator::Job> cancelledJob = cancelledJobs.front();
      std::shared_ptr<Orchestrator::AsyncWorkJob> asyncWorkJob = std::static_pointer_cast<Orchestrator::AsyncWorkJob>(cancelledJob);

      asyncWorkJob->worker->Cancel();
      asyncWorkJob->worker->WorkComplete();
      asyncWorkJob->worker->Destroy();

      cancelledJobs.pop();
    }

    // We need to cancel all callbacks that were scheduled before the shutdown
    // request went through. This will help finish any work any currently operating
    // executors are undertaking
    while (cancelledCallbacks.size()) {
      JSThreadCallback cancelledCallback = cancelledCallbacks.front();
      cancelledCallback.cancel();
      cancelledCallbacks.pop();
    }

    std::for_each(orchestrators.begin(), orchestrators.end(), [](Orchestrator &orchestrator) {
      orchestrator.WaitForThreadClose();
    });

    // After we have completed waiting for all threads to close
    // we will need to cleanup the rest of the completion callbacks
    // from workers that were still running when the shutdown signal
    // was sent
    std::lock_guard<std::mutex> jsThreadLock(*jsThreadCallbackMutex);
    while (jsThreadCallbackQueue.size()) {
      JSThreadCallback jsThreadCallback = jsThreadCallbackQueue.front();
      jsThreadCallback.cancel();
      jsThreadCallbackQueue.pop();
    }

    AsyncCallbackData *asyncCallbackData = static_cast<AsyncCallbackData *>(jsThreadCallbackAsync.data);
    asyncCallbackData->cleanupHandle.swap(cleanupHandle);
    asyncCallbackData->pool = nullptr;

    uv_close(reinterpret_cast<uv_handle_t *>(&jsThreadCallbackAsync), [](uv_handle_t *handle) {
      auto asyncCallbackData = static_cast<AsyncCallbackData *>(handle->data);
      delete asyncCallbackData;
    });
  }

  ThreadPool::ThreadPool(int numberOfThreads, uv_loop_t *loop, nodegit::Context *context)
    : impl(new ThreadPoolImpl(numberOfThreads, loop, context))
  {}

  ThreadPool::~ThreadPool() {}

  void ThreadPool::QueueWorker(nodegit::AsyncWorker *worker) {
    impl->QueueWorker(worker);
  }

  void ThreadPool::PostCallbackEvent(OnPostCallbackFn onPostCallback) {
    Executor::PostCallbackEvent(onPostCallback);
  }

  Nan::AsyncResource *ThreadPool::GetCurrentAsyncResource() {
    return Executor::GetCurrentAsyncResource();
  }

  const nodegit::Context *ThreadPool::GetCurrentContext() {
    return Executor::GetCurrentContext();
  }

  Nan::Global<v8::Value> *ThreadPool::GetCurrentCallbackErrorHandle() {
    return Executor::GetCurrentCallbackErrorHandle();
  }

  void ThreadPool::Shutdown(std::unique_ptr<AsyncContextCleanupHandle> cleanupHandle) {
    impl->Shutdown(std::move(cleanupHandle));
  }

  void ThreadPool::InitializeGlobal() {
    git_custom_tls_set_callbacks(
      Executor::RetrieveTLSForLibgit2ChildThread,
      Executor::SetTLSForLibgit2ChildThread,
      Executor::TeardownTLSOnLibgit2ChildThread
    );
  }
}

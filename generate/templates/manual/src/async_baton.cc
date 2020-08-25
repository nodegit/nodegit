#include "../include/async_baton.h"

namespace nodegit {
  void deleteBaton(AsyncBaton *baton) {
    delete baton;
  }

  AsyncBaton::AsyncBaton()
    : asyncResource(ThreadPool::GetCurrentAsyncResource()), completedMutex(new std::mutex), hasCompleted(false)
  {}

  void AsyncBaton::SignalCompletion() {
    std::lock_guard<std::mutex> lock(*completedMutex);
    hasCompleted = true;
    completedCondition.notify_one();
  }

  void AsyncBaton::Done() {
    onCompletion();
  }

  Nan::AsyncResource *AsyncBaton::GetAsyncResource() {
    return asyncResource;
  }

  void AsyncBaton::ExecuteAsyncPerform(AsyncCallback asyncCallback, CompletionCallback onCompletion) {
    auto jsCallback = [asyncCallback, this]() {
      asyncCallback(this);
    };

    if (onCompletion) {
      this->onCompletion = [this, onCompletion]() {
        onCompletion(this);
      };

      ThreadPool::PostCallbackEvent(
        [this, jsCallback](
          ThreadPool::QueueCallbackFn queueCallback,
          ThreadPool::Callback callbackCompleted
        ) -> ThreadPool::Callback {
          queueCallback(jsCallback);
          callbackCompleted();

          return []() {};
        }
      );
    } else {
      ThreadPool::PostCallbackEvent(
        [this, jsCallback](
          ThreadPool::QueueCallbackFn queueCallback,
          ThreadPool::Callback callbackCompleted
        ) -> ThreadPool::Callback {
          this->onCompletion = callbackCompleted;

          queueCallback(jsCallback);

          return std::bind(&AsyncBaton::SignalCompletion, this);
        }
      );

      WaitForCompletion();
    }
  }

  void AsyncBaton::WaitForCompletion() {
    std::unique_lock<std::mutex> lock(*completedMutex);
    while (!hasCompleted) completedCondition.wait(lock);
  }
}

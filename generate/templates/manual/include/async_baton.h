#ifndef ASYNC_BATON
#define ASYNC_BATON

#include <condition_variable>
#include <memory>
#include <mutex>
#include <nan.h>

#include "lock_master.h"
#include "nodegit.h"
#include "thread_pool.h"

namespace nodegit {
  // Base class for Batons used for callbacks (for example,
  // JS functions passed as callback parameters,
  // or field properties of configuration objects whose values are callbacks)
  class AsyncBaton {
    public:
      typedef std::function<void(void *)> AsyncCallback;
      typedef std::function<void(AsyncBaton *)> CompletionCallback;

      AsyncBaton();
      AsyncBaton(const AsyncBaton &) = delete;
      AsyncBaton(AsyncBaton &&) = delete;
      AsyncBaton &operator=(const AsyncBaton &) = delete;
      AsyncBaton &operator=(AsyncBaton &&) = delete;

      virtual ~AsyncBaton() {}

      void Done();

      Nan::AsyncResource *GetAsyncResource();

      void SetCallbackError(v8::Local<v8::Value> error);

    protected:
      void ExecuteAsyncPerform(AsyncCallback asyncCallback, AsyncCallback asyncCancelCb, CompletionCallback onCompletion);

    private:
      void SignalCompletion();
      void WaitForCompletion();

      Nan::AsyncResource *asyncResource;
      Nan::Global<v8::Value> &callbackErrorHandle;
      ThreadPool::Callback onCompletion;
      std::unique_ptr<std::mutex> completedMutex;
      std::condition_variable completedCondition;
      bool hasCompleted;
  };

  void deleteBaton(AsyncBaton *baton);

  template<typename ResultT>
  class AsyncBatonWithResult : public AsyncBaton {
    public:
      ResultT defaultResult; // result returned if the callback doesn't return anything valid
      ResultT result;

      AsyncBatonWithResult(const ResultT &defaultResult)
        : defaultResult(defaultResult) {
      }

      ResultT ExecuteAsync(AsyncBaton::AsyncCallback asyncCallback, AsyncBaton::AsyncCallback asyncCancelCb, AsyncBaton::CompletionCallback onCompletion = nullptr) {
        result = 0;
        ExecuteAsyncPerform(asyncCallback, asyncCancelCb, onCompletion);
        return result;
      }
  };

  class AsyncBatonWithNoResult : public AsyncBaton {
    public:
      void ExecuteAsync(AsyncBaton::AsyncCallback asyncCallback, AsyncBaton::AsyncCallback asyncCancelCb, AsyncBaton::CompletionCallback onCompletion = nullptr) {
        ExecuteAsyncPerform(asyncCallback, asyncCancelCb, onCompletion);
      }
  };
}

#endif

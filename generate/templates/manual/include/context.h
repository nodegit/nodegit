#ifndef NODEGIT_CONTEXT
#define NODEGIT_CONTEXT

#include <map>
#include <memory>
#include <nan.h>
#include <string>
#include <uv.h>
#include <v8.h>

#include "async_worker.h"
#include "cleanup_handle.h"
#include "thread_pool.h"
#include "tracker_wrap.h"

namespace nodegit {
  class AsyncContextCleanupHandle;
  class Context {
  public:
    Context(v8::Isolate *isolate);
    Context(const Context &) = delete;
    Context(Context &&) = delete;
    Context &operator=(const Context &) = delete;
    Context &operator=(Context &&) = delete;

    ~Context();

    static Context *GetCurrentContext();

    v8::Local<v8::Value> GetFromPersistent(std::string key);

    void QueueWorker(nodegit::AsyncWorker *worker);

    void SaveToPersistent(std::string key, const v8::Local<v8::Value> &value);

    void SaveCleanupHandle(std::string key, std::shared_ptr<nodegit::CleanupHandle> cleanupHandle);

    std::shared_ptr<nodegit::CleanupHandle> GetCleanupHandle(std::string key);

    std::shared_ptr<nodegit::CleanupHandle> RemoveCleanupHandle(std::string key);

    void ShutdownThreadPool(std::unique_ptr<AsyncContextCleanupHandle> cleanupHandle);

    inline void LinkTrackerList(nodegit::TrackerWrap::TrackerList *list) {
      list->Link(&trackerList);
    }

    inline int TrackerListSize() {
      return nodegit::TrackerWrap::SizeFromList(&trackerList);
    }

  private:
    v8::Isolate *isolate;

    ThreadPool threadPool;

    // This map contains persistent handles that need to be cleaned up
    // after the context has been torn down.
    // Often this is used as a context-aware storage cell for `*::InitializeComponent`
    // to store function templates on them.
    Nan::Global<v8::Object> persistentStorage;

    std::map<std::string, std::shared_ptr<CleanupHandle>> cleanupHandles;

    nodegit::TrackerWrap::TrackerList trackerList;

    static std::map<v8::Isolate *, Context *> contexts;
  };

  class AsyncContextCleanupHandle {
    public:
      AsyncContextCleanupHandle(const AsyncContextCleanupHandle &) = delete;
      AsyncContextCleanupHandle(AsyncContextCleanupHandle &&) = delete;
      AsyncContextCleanupHandle &operator=(const AsyncContextCleanupHandle &) = delete;
      AsyncContextCleanupHandle &operator=(AsyncContextCleanupHandle &&) = delete;
      ~AsyncContextCleanupHandle();

    private:
      static void AsyncCleanupContext(void *data, void (*uvCallback)(void *), void *uvCallbackData);

      friend class Context;
      AsyncContextCleanupHandle(v8::Isolate *isolate, Context *context);
      Context *context;
      node::AsyncCleanupHookHandle handle;
      void (*doneCallback)(void *);
      void *doneData;
  };
}

#endif

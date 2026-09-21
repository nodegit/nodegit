#ifndef NODEGIT_CONTEXT
#define NODEGIT_CONTEXT

#include <map>
#include <memory>
#include <napi.h>
#include <node.h>
#include <string>
#include <uv.h>

#include "async_worker.h"
#include "cleanup_handle.h"
#include "thread_pool.h"
#include "tracker_wrap.h"

namespace nodegit {
  class AsyncContextCleanupHandle;
  class Context {
  public:
    Context(Napi::Env env);
    Context(const Context &) = delete;
    Context(Context &&) = delete;
    Context &operator=(const Context &) = delete;
    Context &operator=(Context &&) = delete;

    ~Context();

    static Context *GetCurrentContext();

    Napi::Env Env() const { return env; }

    Napi::Value GetFromPersistent(std::string key);

    void QueueWorker(nodegit::AsyncWorker *worker);

    void SaveToPersistent(std::string key, const Napi::Value &value);

    void SaveCleanupHandle(std::string key, std::shared_ptr<nodegit::CleanupHandle> cleanupHandle);

    std::shared_ptr<nodegit::CleanupHandle> GetCleanupHandle(std::string key);

    std::shared_ptr<nodegit::CleanupHandle> RemoveCleanupHandle(std::string key);

    void ShutdownThreadPool();

    inline void LinkTrackerList(nodegit::TrackerWrap *list) {
      list->Link(&trackerList);
    }

    inline int TrackerListSize() {
      return nodegit::TrackerWrap::SizeFromList(&trackerList);
    }

  private:
    Napi::Env env;

    ThreadPool threadPool;

    // This map contains persistent handles that need to be cleaned up
    // after the context has been torn down.
    // Often this is used as a context-aware storage cell for `*::InitializeComponent`
    // to store function templates on them.
    Napi::ObjectReference persistentStorage;

    std::map<std::string, std::shared_ptr<CleanupHandle>> cleanupHandles;

    nodegit::TrackerWrap::TrackerList trackerList;

    static thread_local Context *currentContext;
  };

  class AsyncContextCleanupHandle {
    public:
      AsyncContextCleanupHandle(const AsyncContextCleanupHandle &) = delete;
      AsyncContextCleanupHandle(AsyncContextCleanupHandle &&) = delete;
      AsyncContextCleanupHandle &operator=(const AsyncContextCleanupHandle &) = delete;
      AsyncContextCleanupHandle &operator=(AsyncContextCleanupHandle &&) = delete;
      ~AsyncContextCleanupHandle();

    private:
      static void AsyncCleanupContext(void *data);

      friend class Context;
      AsyncContextCleanupHandle(Napi::Env env, Context *context);
      Context *context;
  };
}

#endif

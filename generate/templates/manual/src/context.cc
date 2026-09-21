#include "../include/context.h"

namespace nodegit {
  thread_local Context *Context::currentContext = nullptr;

  AsyncContextCleanupHandle::AsyncContextCleanupHandle(Napi::Env env, Context *context)
    : context(context)
  {
    // napi_add_env_cleanup_hook is N-API, unlike node::AddEnvironmentCleanupHook.
    napi_add_env_cleanup_hook(env, AsyncCleanupContext, this);
  }

  AsyncContextCleanupHandle::~AsyncContextCleanupHandle() {
    delete context;
  }

  void AsyncContextCleanupHandle::AsyncCleanupContext(void *data) {
    std::unique_ptr<AsyncContextCleanupHandle> cleanupHandle(static_cast<AsyncContextCleanupHandle *>(data));
    Napi::Env env = cleanupHandle->context->Env();
    Napi::HandleScope scope(env);
    // N-API's cleanup hook cannot be deferred, so ShutdownThreadPool has to
    // complete - including joining every worker thread - before we return.
    // The handle released at the end of this scope owns the Context, so the
    // thread pool is destroyed only after it is done shutting itself down.
    cleanupHandle->context->ShutdownThreadPool();
    cleanupHandle.reset();
  }

  Context::Context(Napi::Env env)
    : env(env),
      threadPool(10, this)
  {
    Napi::Object storage = Napi::Object::New(env);
    persistentStorage = Napi::Persistent(storage);
    currentContext = this;
    new AsyncContextCleanupHandle(env, this);
  }

  Context::~Context() {
    nodegit::TrackerWrap::DeleteFromList(&trackerList);

    if (currentContext == this) {
      currentContext = nullptr;
    }
  }

  std::shared_ptr<CleanupHandle> Context::GetCleanupHandle(std::string key) {
    return cleanupHandles[key];
  }

  Context *Context::GetCurrentContext() {
    return currentContext;
  }

  Napi::Value Context::GetFromPersistent(std::string key) {
    Napi::Object storage = persistentStorage.Value();
    return storage.Get(key);
  }

  void Context::QueueWorker(nodegit::AsyncWorker *worker) {
    threadPool.QueueWorker(worker);
  }

  std::shared_ptr<CleanupHandle> Context::RemoveCleanupHandle(std::string key) {
    std::shared_ptr<CleanupHandle> cleanupItem = cleanupHandles[key];
    cleanupHandles.erase(key);
    return cleanupItem;
  }

  void Context::SaveToPersistent(std::string key, const Napi::Value &value) {
    Napi::Object storage = persistentStorage.Value();
    storage.Set(key, value);
  }

  void Context::SaveCleanupHandle(std::string key, std::shared_ptr<CleanupHandle> cleanupItem) {
    cleanupHandles[key] = cleanupItem;
  }

  void Context::ShutdownThreadPool() {
    threadPool.Shutdown();
  }
}

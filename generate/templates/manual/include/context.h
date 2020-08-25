#ifndef NODEGIT_CONTEXT
#define NODEGIT_CONTEXT

#include <map>
#include <nan.h>
#include <string>
#include <uv.h>
#include <v8.h>

#include "async_worker.h"
#include "thread_pool.h"

namespace nodegit {
  class Context {
  public:
    Context(v8::Isolate *isolate);

    void QueueWorker(nodegit::AsyncWorker *worker);

    void SaveToPersistent(std::string key, const v8::Local<v8::Value> &value);

    v8::Local<v8::Value> GetFromPersistent(std::string key);

    static Context *GetCurrentContext();

    ~Context();

  private:
    v8::Isolate *isolate;

    ThreadPool threadPool;

    // This map contains persistent handles that need to be cleaned up
    // after the context has been torn down.
    // Often this is used as a context-aware storage cell for `*::InitializeComponent`
    // to store function templates on them.
    Nan::Persistent<v8::Object> persistentStorage;

    static std::map<v8::Isolate *, Context *> contexts;
  };
}

#endif

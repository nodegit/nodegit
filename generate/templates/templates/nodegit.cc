#include <node.h>
#include <v8.h>

#include <git2.h>
#include <map>
#include <algorithm>
#include <set>
#include <openssl/crypto.h>
#include <mutex>

#include "../include/init_ssh2.h"
#include "../include/lock_master.h"
#include "../include/nodegit.h"
#include "../include/wrapper.h"
#include "../include/promise_completion.h"
#include "../include/functions/copy.h"
{% each %}
  {% if type != "enum" %}
    #include "../include/{{ filename }}.h"
  {% endif %}
{% endeach %}
#include "../include/convenient_patch.h"
#include "../include/convenient_hunk.h"
#include "../include/filter_registry.h"

using namespace v8;

Local<Value> GetPrivate(Local<Object> object, Local<String> key) {
  Local<Value> value;
  Nan::Maybe<bool> result = Nan::HasPrivate(object, key);
  if (!(result.IsJust() && result.FromJust()))
    return Local<Value>();
  if (Nan::GetPrivate(object, key).ToLocal(&value))
    return value;
  return Local<Value>();
}

void SetPrivate(Local<Object> object, Local<String> key, Local<Value> value) {
  if (value.IsEmpty())
    return;
  Nan::SetPrivate(object, key, value);
}

static uv_mutex_t *opensslMutexes;

void OpenSSL_LockingCallback(int mode, int type, const char *, int) {
  if (mode & CRYPTO_LOCK) {
    uv_mutex_lock(&opensslMutexes[type]);
  } else {
    uv_mutex_unlock(&opensslMutexes[type]);
  }
}

void OpenSSL_IDCallback(CRYPTO_THREADID *id) {
  CRYPTO_THREADID_set_numeric(id, (unsigned long)uv_thread_self());
}

void OpenSSL_ThreadSetup() {
  opensslMutexes=(uv_mutex_t *)malloc(CRYPTO_num_locks() * sizeof(uv_mutex_t));

  for (int i=0; i<CRYPTO_num_locks(); i++) {
    uv_mutex_init(&opensslMutexes[i]);
  }

  CRYPTO_set_locking_callback(OpenSSL_LockingCallback);
  CRYPTO_THREADID_set_callback(OpenSSL_IDCallback);
}

// TODO initialize a thread pool per context. Replace uv_default_loop() with node::GetCurrentEventLoop(isolate);
ThreadPool libgit2ThreadPool(10, uv_default_loop());

static std::once_flag libraryInitializedFlag;
static std::mutex libraryInitializationMutex;

NAN_MODULE_INIT(init) {
  {
    // We only want to do initialization logic once, and we also want to prevent any thread from completely loading
    // the module until initialization has occurred.
    // All of this initialization logic ends up being shared.
    const std::lock_guard<std::mutex> lock(libraryInitializationMutex);
    std::call_once(libraryInitializedFlag, []() {
      // Initialize thread safety in openssl and libssh2
      OpenSSL_ThreadSetup();
      init_ssh2();
      // Initialize libgit2.
      git_libgit2_init();

      LockMaster::InitializeGlobal();
    });
  }

  Nan::HandleScope scope;

  Wrapper::InitializeComponent(target);
  PromiseCompletion::InitializeComponent();
  {% each %}
    {% if type != "enum" %}
      {{ cppClassName }}::InitializeComponent(target);
    {% endif %}
  {% endeach %}

  ConvenientHunk::InitializeComponent(target);
  ConvenientPatch::InitializeComponent(target);
  GitFilterRegistry::InitializeComponent(target);

  LockMaster::InitializeContext();
}

NODE_MODULE(nodegit, init)

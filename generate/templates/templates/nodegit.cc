#include <string_view>

#include <node.h>
#include <napi.h>
#include <v8.h>

#include <openssl/opensslv.h>

#include <git2.h>
#include <map>
#include <algorithm>
#include <set>
#include <mutex>

#include "../include/init_ssh2.h"
#include "../include/lock_master.h"
#include "../include/nodegit.h"
#include "../include/context.h"
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

Napi::Value GetPrivate(Napi::Object object, Napi::String key) {
  return object.Get(key);
}

void SetPrivate(Napi::Object object, Napi::String key, Napi::Value value) {
  if (value.IsUndefined())
    return;
  object.Set(key, value);
}

// diagnostic function
Napi::Value GetNumberOfTrackedObjects(const Napi::CallbackInfo& info) {
  nodegit::Context *currentNodeGitContext = nodegit::Context::GetCurrentContext();
  assert (currentNodeGitContext != nullptr);
  return Napi::Number::New(info.Env(), currentNodeGitContext->TrackerListSize());
}

static std::once_flag libraryInitializedFlag;
static std::mutex libraryInitializationMutex;

Napi::Object init(Napi::Env env, Napi::Object exports) {
  {
    // We only want to do initialization logic once, and we also want to prevent any thread from completely loading
    // the module until initialization has occurred.
    // All of this initialization logic ends up being shared.
    const std::lock_guard<std::mutex> lock(libraryInitializationMutex);
    std::call_once(libraryInitializedFlag, []() {
      // Initialize thread safety in openssl and libssh2
      init_ssh2();
      // Initialize libgit2.
      git_libgit2_init();

      // Register thread pool with libgit2
      nodegit::ThreadPool::InitializeGlobal();
    });
  }

  // Exports function 'getNumberOfTrackedObjects'
  exports.Set("getNumberOfTrackedObjects", Napi::Function::New(env, GetNumberOfTrackedObjects));

  nodegit::Context *nodegitContext = new nodegit::Context(env);

  Wrapper::InitializeComponent(env, exports, nodegitContext);
  PromiseCompletion::InitializeComponent(env, nodegitContext);
  {% each %}
    {% if type == 'class' %}
      {{ cppClassName }}::InitializeComponent(exports, nodegitContext);
    {% elsif type == 'struct' %}
    {% if isReturnable %}
      {{ cppClassName }}::InitializeComponent(exports, nodegitContext);
    {% endif %}
    {% endif %}
  {% endeach %}

  ConvenientHunk::InitializeComponent(exports, nodegitContext);
  ConvenientPatch::InitializeComponent(exports, nodegitContext);
  nodegit::GitFilterRegistry::InitializeComponent(exports, nodegitContext);

  nodegit::LockMaster::InitializeContext(env);

  env.AddCleanupHook([]() {
#if OPENSSL_VERSION_NUMBER < 0x10100000L
    CRYPTO_set_locking_callback(NULL);
    CRYPTO_THREADID_set_callback(NULL);
#endif
  });

  return exports;
}

NODE_API_MODULE(nodegit, init)

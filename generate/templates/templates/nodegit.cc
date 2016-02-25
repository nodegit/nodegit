#include <node.h>
#include <v8.h>

#include <git2.h>
#include <map>
#include <algorithm>
#include <set>

#include <openssl/crypto.h>

#include "../include/init_ssh2.h"
#include "../include/lock_master.h"
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

void LockMasterEnable(const FunctionCallbackInfo<Value>& info) {
  LockMaster::Enable();
}

void LockMasterDisable(const FunctionCallbackInfo<Value>& info) {
  LockMaster::Disable();
}

void LockMasterIsEnabled(const FunctionCallbackInfo<Value>& info) {
  info.GetReturnValue().Set(Nan::New(LockMaster::IsEnabled()));
}

void LockMasterGetDiagnostics(const FunctionCallbackInfo<Value>& info) {
  LockMaster::Diagnostics diagnostics(LockMaster::GetDiagnostics());

  // return a plain JS object with properties
  v8::Local<v8::Object> result = Nan::New<v8::Object>();
  result->Set(Nan::New("storedMutexesCount").ToLocalChecked(), Nan::New(diagnostics.storedMutexesCount));
  info.GetReturnValue().Set(result);
}

static uv_mutex_t *opensslMutexes;

void OpenSSL_LockingCallback(int mode, int type, const char *, int) {
  if (mode & CRYPTO_LOCK) {
    uv_mutex_lock(&opensslMutexes[type]);
  } else {
    uv_mutex_unlock(&opensslMutexes[type]);
  }
}

unsigned long OpenSSL_IDCallback() {
  return (unsigned long)uv_thread_self();
}

void OpenSSL_ThreadSetup() {
  opensslMutexes=(uv_mutex_t *)malloc(CRYPTO_num_locks() * sizeof(uv_mutex_t));

  for (int i=0; i<CRYPTO_num_locks(); i++) {
    uv_mutex_init(&opensslMutexes[i]);
  }

  CRYPTO_set_locking_callback(OpenSSL_LockingCallback);
  CRYPTO_set_id_callback(OpenSSL_IDCallback);
}

extern "C" void init(Local<v8::Object> target) {
  // Initialize thread safety in openssl and libssh2
  OpenSSL_ThreadSetup();
  init_ssh2();
  // Initialize libgit2.
  git_libgit2_init();

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

  NODE_SET_METHOD(target, "enableThreadSafety", LockMasterEnable);
  NODE_SET_METHOD(target, "disableThreadSafety", LockMasterDisable);
  NODE_SET_METHOD(target, "isThreadSafetyEnabled", LockMasterIsEnabled);
  NODE_SET_METHOD(target, "getThreadSafetyDiagnostics", LockMasterGetDiagnostics);

  LockMaster::Initialize();
}

NODE_MODULE(nodegit, init)

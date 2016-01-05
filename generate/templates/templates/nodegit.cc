// This is a generated file, modify: generate/templates/nodegit.cc.
#include <v8.h>
#include <node.h>
#include <git2.h>
#include <map>
#include <algorithm>
#include <set>

#include "../include/lock_master.h"
#include "../include/wrapper.h"
#include "../include/promise_completion.h"
#include "../include/functions/copy.h"
{% each %}
  {% if type != "enum" %}
    #include "../include/{{ filename }}.h"
  {% endif %}
{% endeach %}

void LockMasterEnable(const FunctionCallbackInfo<Value>& args) {
  LockMaster::Enable();
}

void LockMasterDisable(const FunctionCallbackInfo<Value>& args) {
  LockMaster::Disable();
}

extern "C" void init(Local<v8::Object> target) {
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

  NODE_SET_METHOD(target, "enableThreadSafety", LockMasterEnable);
  NODE_SET_METHOD(target, "disableThreadSafety", LockMasterDisable);

  LockMaster::Initialize();
}

NODE_MODULE(nodegit, init)

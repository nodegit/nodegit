// This is a generated file, modify: generate/templates/nodegit.cc.
#include <v8.h>
#include <node.h>
#include <git2.h>
#include <uv.h>
#include <map>
#include <algorithm>
#include <set>

#include "../include/nodegit.h"
#include "../include/wrapper.h"
#include "../include/functions/copy.h"
{% each %}
  {% if type != "enum" %}
    #include "../include/{{ filename }}.h"
  {% endif %}
{% endeach %}

std::map<const void *, uv_mutex_t *> mutexes;
uv_mutex_t map_mutex;
uv_key_t current_lock_master_key;

extern "C" void init(Local<v8::Object> target) {
  // Initialize libgit2.
  git_libgit2_init();

  Nan::HandleScope scope;

  Wrapper::InitializeComponent(target);
  {% each %}
    {% if type != "enum" %}
      {{ cppClassName }}::InitializeComponent(target);
    {% endif %}
  {% endeach %}

  uv_mutex_init(&map_mutex);
  uv_key_create(&current_lock_master_key);
}

void LockMaster::GetMutexes()
{
  uv_mutex_lock(&map_mutex);

  for (std::set<const void *>::const_iterator it = objects_to_lock.begin(); it != objects_to_lock.end(); it++) {
    const void *object = *it;
    if(object) {
      // ensure we have an initialized mutex for each object
      if(!mutexes[object]) {
        mutexes[object] = (uv_mutex_t *)malloc(sizeof(uv_mutex_t));
        uv_mutex_init(mutexes[object]);
      }

      object_mutexes.push_back(mutexes[object]);
    }
  }

  uv_mutex_unlock(&map_mutex);
}

void LockMaster::Register()
{
  uv_key_set(&current_lock_master_key, this);
}

void LockMaster::Unregister()
{
  uv_key_set(&current_lock_master_key, NULL);
}

void LockMaster::Lock()
{
  // lock the mutex (note locking the mutexes one by one can lead to
  // deadlocks... we might be better off locking them all at once
  // (using trylock, then unlocking if they can't all be locked, then lock & wait, repeat...)
  std::for_each(object_mutexes.begin(), object_mutexes.end(), uv_mutex_lock);
}

void LockMaster::Unlock()
{
  std::for_each(object_mutexes.begin(), object_mutexes.end(), uv_mutex_unlock);
}

LockMaster::TemporaryUnlock::TemporaryUnlock() {
  lockMaster = (LockMaster *)uv_key_get(&current_lock_master_key);
  lockMaster->Unlock();
}

LockMaster::TemporaryUnlock::~TemporaryUnlock() {
  lockMaster->Lock();
}

NODE_MODULE(nodegit, init)

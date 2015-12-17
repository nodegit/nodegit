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
}

std::vector<uv_mutex_t *> get_mutexes(const std::set<const void *> libgit2_objects)
{
  std::vector<uv_mutex_t *> result;

  uv_mutex_lock(&map_mutex);

  for (std::set<const void *>::const_iterator it = libgit2_objects.begin(); it != libgit2_objects.end(); it++) {
    const void *object = *it;
    if(object) {
      // ensure we have an initialized mutex for each object
      if(!mutexes[object]) {
        mutexes[object] = (uv_mutex_t *)malloc(sizeof(uv_mutex_t));
        uv_mutex_init(mutexes[object]);
      }

      result.push_back(mutexes[object]);
    }
  }

  uv_mutex_unlock(&map_mutex);

  return result;
}

void LockMaster::Lock()
{
  object_mutexes = get_mutexes(objects_to_lock);

  // lock the mutex (note locking the mutexes one by one can lead to
  // deadlocks... we might be better off locking them all at once
  // (using trylock, then unlocking if they can't all be locked, then lock & wait, repeat...)
  std::for_each(object_mutexes.begin(), object_mutexes.end(), uv_mutex_lock);
}

void LockMaster::Unlock()
{
  std::for_each(object_mutexes.begin(), object_mutexes.end(), uv_mutex_unlock);
}

NODE_MODULE(nodegit, init)

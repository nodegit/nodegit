#include <git2.h>
#include <uv.h>
#include <map>
#include "../include/lock_master.h"

std::map<const void *, std::pair<uv_mutex_t *, unsigned> > mutexes;
uv_mutex_t map_mutex;
uv_key_t current_lock_master_key;
uv_async_t cleanup_mutexes_handle;

void cleanup_mutexes(uv_async_t *async) {
  uv_mutex_lock(&map_mutex);

  for(std::map<const void *, std::pair<uv_mutex_t *, unsigned> >::iterator it=mutexes.begin(); it != mutexes.end(); )
  {
    uv_mutex_t *mutex = it->second.first;
    unsigned use_count = it->second.second;
    // if the mutex is only referenced by the mutexes map,
    // we can destroy it (because any LockMaster that is using the mutex would
    // hold it in its object_mutexes)
    if (!use_count) {
      uv_mutex_destroy(mutex);
      free(mutex);
      std::map<const void *, std::pair<uv_mutex_t *, unsigned> >::iterator to_erase = it;
      it++;
      mutexes.erase(to_erase);
    } else {
      it++;
    }
  }

  uv_mutex_unlock(&map_mutex);
}

void LockMaster::Initialize() {
  uv_mutex_init(&map_mutex);
  uv_key_create(&current_lock_master_key);
  uv_async_init(uv_default_loop(), &cleanup_mutexes_handle, cleanup_mutexes);
}

std::vector<uv_mutex_t *> LockMaster::GetMutexes(int use_count_delta)
{
  std::vector<uv_mutex_t *> object_mutexes;

  uv_mutex_lock(&map_mutex);

  for (std::set<const void *>::const_iterator it = objects_to_lock.begin(); it != objects_to_lock.end(); it++) {
    const void *object = *it;
    if(object) {
      // ensure we have an initialized mutex for each object
      std::map<const void *, std::pair<uv_mutex_t *, unsigned> >::iterator mutex_it = mutexes.find(object);
      if(mutex_it == mutexes.end()) {
        mutex_it = mutexes.insert(
          std::make_pair(
            object,
            std::make_pair((uv_mutex_t *)malloc(sizeof(uv_mutex_t)), 0U)
          )
        ).first;
        uv_mutex_init(mutex_it->second.first);
      }

      object_mutexes.push_back(mutex_it->second.first);
      mutex_it->second.second += use_count_delta;
    }
  }

  uv_mutex_unlock(&map_mutex);

  return object_mutexes;
}

void LockMaster::Register()
{
  uv_key_set(&current_lock_master_key, this);
}

void LockMaster::Unregister()
{
  uv_key_set(&current_lock_master_key, NULL);
}

void LockMaster::Lock(bool acquire_mutexes)
{
  std::vector<uv_mutex_t *> object_mutexes = GetMutexes(acquire_mutexes * 1);

  std::vector<uv_mutex_t *>::iterator already_locked = object_mutexes.end();

  // we will attempt to lock all the mutexes at the same time to avoid deadlocks
  // note in most cases we are locking 0 or 1 mutexes. more than 1 implies
  // passing objects with different repos/owners in the same call.
  std::vector<uv_mutex_t *>::iterator it;
  do
  {
    // go through all the mutexes and try to lock them
    for(it = object_mutexes.begin(); it != object_mutexes.end(); it++) {
      // if we already locked this mutex in a previous pass via uv_mutex_lock,
      // we don't need to lock it again
      if(it == already_locked) {
        continue;
      }
      // first, try to lock (non-blocking)
      bool failure = uv_mutex_trylock(*it);
      if(failure) {
        // we have failed to lock a mutex... unlock everything we have locked
        for(std::vector<uv_mutex_t *>::iterator unlock_it = object_mutexes.begin(); unlock_it != it; unlock_it++) {
          uv_mutex_unlock(*unlock_it);
        }
        if(already_locked > it && already_locked != object_mutexes.end()) {
          uv_mutex_unlock(*already_locked);
        }
        // now do a blocking lock on what we couldn't lock
        uv_mutex_lock(*it);
        // mark that we have already locked this one
        // if there are more mutexes than this one, we will go back to locking everything
        already_locked = it;
        break;
      }
    }
  } while(it != object_mutexes.end());
}

void LockMaster::Unlock(bool release_mutexes)
{
  std::vector<uv_mutex_t *> object_mutexes = GetMutexes(release_mutexes * -1);

  for(std::vector<uv_mutex_t *>::iterator it=object_mutexes.begin(); it != object_mutexes.end(); it++) {
    uv_mutex_unlock(*it);
  }
}

void LockMaster::CleanupMutexes()
{
  // schedule mutex cleanup on the main event loop
  // this somewhat delays and debounces cleanup (uv_async_send coalesces calls)
  uv_async_send(&cleanup_mutexes_handle);
}

LockMaster::TemporaryUnlock::TemporaryUnlock() {
  if(!enabled) {
    return;
  }
  lockMaster = (LockMaster *)uv_key_get(&current_lock_master_key);
  lockMaster->Unlock(false);
}

LockMaster::TemporaryUnlock::~TemporaryUnlock() {
  if(!enabled) {
    return;
  }
  lockMaster->Lock(false);
}

bool LockMaster::enabled = false;

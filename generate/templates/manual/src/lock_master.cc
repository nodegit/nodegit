#include <nan.h>
#include <git2.h>
#include <uv.h>
#include <set>
#include <vector>
#include <map>
#include <algorithm>

#include "../include/lock_master.h"

// information about a lockable object
// - the mutex used to lock it and the number of outstanding locks
struct ObjectInfo {
  uv_mutex_t *mutex;
  unsigned useCount;

  ObjectInfo(uv_mutex_t *mutex, unsigned useCount)
    : mutex(mutex), useCount(useCount)
  {}
};

// LockMaster implementation details
// implemented in a separate class to keep LockMaster opaque
class LockMasterImpl {
  // STATIC variables / methods

  // A map from objects that are locked (or were locked), to information on their mutex
  static std::map<const void *, ObjectInfo> mutexes;
  // A mutex used for the mutexes map
  static uv_mutex_t mapMutex;

  // A libuv key used to store the current thread-specific LockMasterImpl instance
  static uv_key_t currentLockMasterKey;

  // Cleans up any mutexes that are not currently used
  static NAN_GC_CALLBACK(CleanupMutexes);

public:
  static void Initialize();

  // INSTANCE variables / methods

private:
  // The set of objects this LockMaster is responsible for locking
  std::set<const void *> objectsToLock;

  // Mutexes locked by this LockMaster on construction and unlocked on destruction
  std::vector<uv_mutex_t *> GetMutexes(int useCountDelta);
  void Register();
  void Unregister();

public:
  static LockMasterImpl *CurrentLockMasterImpl() {
    return (LockMasterImpl *)uv_key_get(&currentLockMasterKey);
  }
  static LockMaster::Diagnostics GetDiagnostics();

  LockMasterImpl() {
    Register();
  }

  ~LockMasterImpl() {
    Unregister();
    Unlock(true);
  }

  void ObjectToLock(const void *objectToLock) {
    objectsToLock.insert(objectToLock);
  }

  void Lock(bool acquireMutexes);
  void Unlock(bool releaseMutexes);
};

std::map<const void *, ObjectInfo> LockMasterImpl::mutexes;
uv_mutex_t LockMasterImpl::mapMutex;
uv_key_t LockMasterImpl::currentLockMasterKey;

void LockMasterImpl::Initialize() {
  uv_mutex_init(&mapMutex);
  uv_key_create(&currentLockMasterKey);
  Nan::AddGCEpilogueCallback(CleanupMutexes);
}

NAN_GC_CALLBACK(LockMasterImpl::CleanupMutexes) {
  // skip cleanup if thread safety is disabled
  // this means that turning thread safety on and then off
  // could result in remaining mutexes - but they would get cleaned up
  // if thread safety is turned on again
  if (LockMaster::GetStatus() == LockMaster::Disabled) {
    return;
  }

  uv_mutex_lock(&mapMutex);

  for (auto it = mutexes.begin(); it != mutexes.end(); )
  {
    uv_mutex_t *mutex = it->second.mutex;
    unsigned useCount = it->second.useCount;
    // if the mutex is not used by any LockMasters,
    // we can destroy it
    if (!useCount) {
      uv_mutex_destroy(mutex);
      free(mutex);
      auto to_erase = it;
      it++;
      mutexes.erase(to_erase);
    } else {
      it++;
    }
  }

  uv_mutex_unlock(&mapMutex);
}

void LockMaster::Initialize() {
  LockMasterImpl::Initialize();
}

std::vector<uv_mutex_t *> LockMasterImpl::GetMutexes(int useCountDelta) {
  std::vector<uv_mutex_t *> objectMutexes;

  uv_mutex_lock(&mapMutex);

  for (auto object : objectsToLock) {
    if(object) {
      // ensure we have an initialized mutex for each object
      auto mutexIt = mutexes.find(object);
      if(mutexIt == mutexes.end()) {
        mutexIt = mutexes.insert(
          std::make_pair(
            object,
            ObjectInfo((uv_mutex_t *)malloc(sizeof(uv_mutex_t)), 0U)
          )
        ).first;
        uv_mutex_init(mutexIt->second.mutex);
      }

      objectMutexes.push_back(mutexIt->second.mutex);
      mutexIt->second.useCount += useCountDelta;
    }
  }

  uv_mutex_unlock(&mapMutex);

  return objectMutexes;
}

void LockMasterImpl::Register() {
  uv_key_set(&currentLockMasterKey, this);
}

void LockMasterImpl::Unregister() {
  uv_key_set(&currentLockMasterKey, NULL);
}

void LockMasterImpl::Lock(bool acquireMutexes) {
  std::vector<uv_mutex_t *> objectMutexes = GetMutexes(acquireMutexes * 1);

  auto alreadyLocked = objectMutexes.end();

  // we will attempt to lock all the mutexes at the same time to avoid deadlocks
  // note in most cases we are locking 0 or 1 mutexes. more than 1 implies
  // passing objects with different repos/owners in the same call.
  std::vector<uv_mutex_t *>::iterator it;
  do {
    // go through all the mutexes and try to lock them
    for(it = objectMutexes.begin(); it != objectMutexes.end(); it++) {
      // if we already locked this mutex in a previous pass via uv_mutex_lock,
      // we don't need to lock it again
      if (it == alreadyLocked) {
        continue;
      }
      // first, try to lock (non-blocking)
      bool failure = uv_mutex_trylock(*it);
      if(failure) {
        // we have failed to lock a mutex... unlock everything we have locked
        std::for_each(objectMutexes.begin(), it, uv_mutex_unlock);
        if (alreadyLocked > it && alreadyLocked != objectMutexes.end()) {
          uv_mutex_unlock(*alreadyLocked);
        }
        // now do a blocking lock on what we couldn't lock
        uv_mutex_lock(*it);
        // mark that we have already locked this one
        // if there are more mutexes than this one, we will go back to locking everything
        alreadyLocked = it;
        break;
      }
    }
  } while(it != objectMutexes.end());
}

void LockMasterImpl::Unlock(bool releaseMutexes) {
  // Get the mutexes but don't decrement their use count until after we've
  // unlocked them all.
  std::vector<uv_mutex_t *> objectMutexes = GetMutexes(0);

  std::for_each(objectMutexes.begin(), objectMutexes.end(), uv_mutex_unlock);

  GetMutexes(releaseMutexes * -1);
}

LockMaster::Diagnostics LockMasterImpl::GetDiagnostics() {
  LockMaster::Diagnostics diagnostics;
  uv_mutex_lock(&LockMasterImpl::mapMutex);
  diagnostics.storedMutexesCount = mutexes.size();
  uv_mutex_unlock(&LockMasterImpl::mapMutex);
  return diagnostics;
}

// LockMaster

void LockMaster::ConstructorImpl() {
  impl = new LockMasterImpl();
}

void LockMaster::DestructorImpl() {
  delete impl;
}

void LockMaster::ObjectToLock(const void *objectToLock) {
  impl->ObjectToLock(objectToLock);
}

void LockMaster::ObjectsToLockAdded() {
  impl->Lock(true);
}

LockMaster::Diagnostics LockMaster::GetDiagnostics() {
  return LockMasterImpl::GetDiagnostics();
}

// LockMaster::TemporaryUnlock

void LockMaster::TemporaryUnlock::ConstructorImpl() {
  impl = LockMasterImpl::CurrentLockMasterImpl();
  if(impl) {
    impl->Unlock(false);
  }
}

void LockMaster::TemporaryUnlock::DestructorImpl() {
  impl->Lock(false);
}

LockMaster::Status LockMaster::status = LockMaster::Disabled;
